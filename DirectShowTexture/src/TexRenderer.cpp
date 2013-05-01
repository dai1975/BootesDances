#include "TexRenderer.h"

namespace DirectShowTexture {

// {DB912966-3067-4d35-9175-AFB1B92AB2DA}
static const GUID CLSID_MYRENDERER = 
{ 0xdb912966, 0x3067, 0x4d35, { 0x91, 0x75, 0xaf, 0xb1, 0xb9, 0x2a, 0xb2, 0xda } };

Renderer::Renderer(HRESULT *phr)
: CBaseVideoRenderer(CLSID_MYRENDERER, _T("TexRenderer"), NULL, phr)
{ }

Renderer::~Renderer()
{ }

TexRenderer::TexRenderer(IDirect3DDevice9* pDev, D3DPOOL pool, HRESULT *phr)
: Renderer(phr)
, m_pDev(pDev)
{
   m_mutex = CreateMutex(NULL, FALSE, NULL);
	m_pTexInputPin = new TexInputPin(pool, this, &m_InterfaceLock, phr, _T("TexInputPin"));
	m_pTexInputPin->AddRef();
	m_pInputPin = m_pTexInputPin;
	m_pInputPin->AddRef();

	m_pLatestSample = NULL;
   for (int i=0; i<2; ++i) { m_tex[i].pTex = NULL; }
   m_iTake = 0;
   m_update = true;
}

TexRenderer::~TexRenderer()
{
	m_pInputPin->Release();
	m_pTexInputPin->Release();

	if (m_pLatestSample) {
		m_pLatestSample->Release();
		m_pLatestSample = NULL;
	}
   for (int i=0; i<2; ++i) {
      if (m_tex[i].pTex) { 
         m_tex[i].pTex->Release();
         m_tex[i].pTex = NULL;
      }
   }

   CloseHandle(m_mutex);
}


HRESULT TexRenderer::CheckMediaType(const CMediaType *pMediaType)
{
	return m_pTexInputPin->getTexAllocator()->CheckMediaType(m_pDev, pMediaType);
}

HRESULT TexRenderer::SetMediaType(const CMediaType *pMT)
{
   for (int i=0; i<2; ++i) {
      if (m_tex[i].pTex) {
         m_tex[i].pTex->Release();
         m_tex[i].pTex = NULL;
      }
   }
   HRESULT hr;
   {
      hr = m_pTexInputPin->getTexAllocator()->SetMediaType(m_pDev, pMT, &m_tex[0].pTex);
      if (FAILED(hr)) { return hr; }
   }
   for (int i=1; i<2; ++i) {
      hr = m_pTexInputPin->getTexAllocator()->CreateTexture(&m_tex[i].pTex);
      if (FAILED(hr)) { return hr; }
   }
   m_iTake = 0;
   m_update = true;
   m_pTexInputPin->getTexAllocator()->GetVideoInfo(&m_initialInfo);
   return hr;
}

void TexRenderer::GetInitialVideoInfo(VideoInfo* p)
{
   memcpy(p, &m_initialInfo, sizeof(VideoInfo));
}

// Called when the source delivers us a sample. We go through a few checks to
// make sure the sample can be rendered. If we are running (streaming) then we
// have the sample scheduled with the reference clock, if we are not streaming
// then we have received an sample in paused mode so we can complete any state
// transition. On leaving this function everything will be unlocked so an app
// thread may get in and change our state to stopped (for example) in which
// case it will also signal the thread event so that our wait call is stopped
HRESULT TexRenderer::PrepareReceive(IMediaSample *pMediaSample)
{
/*
	AM_MEDIA_TYPE* p = m_pInputPin->SampleProps()->pMediaType;
	HRESULT hr = CBaseRenderer::PrepareReceive(pMediaSample);
	m_pInputPin->SampleProps()->pMediaType = p;
	return hr;
*/
    CAutoLock cInterfaceLock(&m_InterfaceLock);
    m_bInReceive = TRUE;

    // Check our flushing and filter state

    // This function must hold the interface lock because it calls 
    // CBaseInputPin::Receive() and CBaseInputPin::Receive() uses
    // CBasePin::m_bRunTimeError.
    HRESULT hr = m_pInputPin->CBaseInputPin::Receive(pMediaSample);

    if (hr != NOERROR) {
        m_bInReceive = FALSE;
        return E_FAIL;
    }

    // Has the type changed on a media sample. We do all rendering
    // synchronously on the source thread, which has a side effect
    // that only one buffer is ever outstanding. Therefore when we
    // have Receive called we can go ahead and change the format
    // Since the format change can cause a SendMessage we just don't
    // lock
/*
    if (m_pInputPin->SampleProps()->pMediaType) {
        hr = m_pInputPin->SetMediaType(
                (CMediaType *)m_pInputPin->SampleProps()->pMediaType);
        if (FAILED(hr)) {
            m_bInReceive = FALSE;
            return hr;
        }
    }
*/

    CAutoLock cSampleLock(&m_RendererLock);

    ASSERT(IsActive() == TRUE);
    ASSERT(m_pInputPin->IsFlushing() == FALSE);
    ASSERT(m_pInputPin->IsConnected() == TRUE);
    ASSERT(m_pMediaSample == NULL);

    // Return an error if we already have a sample waiting for rendering
    // source pins must serialise the Receive calls - we also check that
    // no data is being sent after the source signalled an end of stream

    if (m_pMediaSample || m_bEOS || m_bAbort) {
        Ready();
        m_bInReceive = FALSE;
        return E_UNEXPECTED;
    }

    // Store the media times from this sample
    if (m_pPosition) m_pPosition->RegisterMediaTime(pMediaSample);

    // Schedule the next sample if we are streaming

    if ((m_bStreaming == TRUE) && (ScheduleSample(pMediaSample) == FALSE)) {
        ASSERT(WAIT_TIMEOUT == WaitForSingleObject((HANDLE)m_RenderEvent,0));
        ASSERT(CancelNotification() == S_FALSE);
        m_bInReceive = FALSE;
        return VFW_E_SAMPLE_REJECTED;
    }

    // Store the sample end time for EC_COMPLETE handling
    m_SignalTime = m_pInputPin->SampleProps()->tStop;

    // BEWARE we sometimes keep the sample even after returning the thread to
    // the source filter such as when we go into a stopped state (we keep it
    // to refresh the device with) so we must AddRef it to keep it safely. If
    // we start flushing the source thread is released and any sample waiting
    // will be released otherwise GetBuffer may never return (see BeginFlush)

    m_pMediaSample = pMediaSample;
    m_pMediaSample->AddRef();

    if (m_bStreaming == FALSE) {
        SetRepaintStatus(TRUE);
    }
    return NOERROR;
}

/*
MediaSample の扱い:
  最上流レンダラの MediaSample 確保処理では、参照がなくなった MediaSample を再利用する(m_lFree)。
  二枚の MediaSample があれば、常にどちらかはフリーになっているので停止せず動く。

  DoRenderSample() で上流から MediaSample を受け取る。
  TexRenderer では最新の一枚を保持しておく(m_pLatestSample)
  具体的には、DoRenderSample() で、現在保持しているサンプルを Release し、貰ったサンプルを AddRef する。


サンプルとテクスチャ:
  MediaSample は自前の TexMediaSample と標準のサンプルの二つの型がある。基本的に前者だが、エラー時には後者で来る。
  TexMediaSample は内部にテクスチャを持っている。
  TexRenderer はサンプルをテクスチャにして扱う。
  TexMediaSample の場合は自前のテクスチャと内部テクスチャと差し替えるだけですむが、
  DefaultMediaSample ではピクセルのコピーを行う。詳しくは後述。
  ここで両者の差は隠蔽され、後は「最新のサンプルの画を持ったテクスチャ」を扱えばよい。

API:
  アプリには TakeTexture, ReleaseTexture, CreateTexture, SwapTexture を提供。
  Take では内部で持っている最新サンプルの画のテクスチャを渡す。カウントは加えておく。
  使い終わったら Release を呼ぶ。引数は不要。
  Take..Release は入れ子にしてはいけない。(Take,Take はダメ)

  Swap はテクスチャを渡して、TexRenderer 内部の最新画テクスチャと交換する。
  渡すテクスチャは CreateTexture で作ったもの。
  Swap の場合は Release を呼ぶ必要はない。
  以前の Swap からサンプルが更新されていない状態で新たに Swap した場合、テクスチャは交換されない。

TexAllocator の場合:
  MediaSample はそのまま最新への参照を持っておく。
  さらに、自前でテクスチャを一つ持っておく。
  Take されたら、自前のテクスチャと MediaSample 内のテクスチャを差し替える。
  新たに自分の管理下となったテクスチャ(Takeで与えられたテクスチャ)をアプリケーションに渡す。
  メディアサンプルはもう使われないので、DoRenderSample 時に交換して解放・再利用してよい。
  一つのサンプルを複数回テクスチャ交換すると不味いので、交換されたことを覚えておく。
  Release 時には特になにもしなくてよい。

それ以外のアロケータの場合:
  MediaSample の内部構造が不明であり、テクスチャ差し替えはできない。
  外に渡すのはテクスチャなので、ビットマップのコピーをする必要がある。
  MediaSample が複数枚ある保証も無いので、一つを保持しておくことはできない。
  DoRenderSample() の中で自前のテクスチャへコピーすることになる。
  さらに、外部へ提供している間はテクスチャは変更できない。  
  DoRenderSample() で止めたくは無いので、外部提供用と描画用の二枚のテクスチャを用意する。
  外部提供中とコピー中では差し替えはできない。
  単純にそれぞれが終わって可能なら交換とすると、最悪の場合ずっと古い絵になってしまう。
  コピー完了時に外部提供中だったら少し待つのがよいかな。
  ここでは単純に、両方の関数全体を排他にする。
*/
HRESULT TexRenderer::CreateTexture(IDirect3DTexture9** ppTex, RectInfo* pRect)
{
   TexAllocator* p = m_pTexInputPin->getTexAllocator();
   HRESULT hr;
   if (FAILED(hr = p->CreateTexture(ppTex))) { return hr; }
   p->GetRectInfo(pRect);
   return hr;
}

HRESULT TexRenderer::SwapTexture(IDirect3DTexture9** ppTex, RectInfo* pInfo, bool* update)
{
	if (m_pTexInputPin->isTexAllocated()) {
      return SwapTexture_Tex(ppTex, pInfo, update);
   } else {
      return SwapTexture_Default(ppTex, pInfo, update);
   }
}
HRESULT TexRenderer::TakeTexture(IDirect3DTexture9** ppTex, RectInfo* pInfo, bool* update)
{
	if (m_pTexInputPin->isTexAllocated()) {
      return TakeTexture_Tex(ppTex, pInfo, update);
   } else {
      return TakeTexture_Default(ppTex, pInfo, update);
   }
}
HRESULT TexRenderer::ReleaseTexture()
{
	if (m_pTexInputPin->isTexAllocated()) {
      return ReleaseTexture_Tex();
   } else {
      return ReleaseTexture_Default();
   }
}
void TexRenderer::OnReceiveFirstSample(IMediaSample *pMediaSample)
{
   DoRenderSample(pMediaSample);
}
HRESULT TexRenderer::DoRenderSample(IMediaSample *pMediaSample)
{
	if (m_pTexInputPin->isTexAllocated()) {
      return DoRenderSample_Tex(pMediaSample);
   } else {
      return DoRenderSample_Default(pMediaSample);
   }
}

//-----------------------
HRESULT TexRenderer::TakeTexture_Tex(IDirect3DTexture9** ppTex, RectInfo* pInfo, bool* update)
{
   HRESULT hr = E_FAIL;
   WaitForSingleObjectEx(m_mutex, INFINITE, true);

   {
      if (m_pLatestSample == NULL) { goto fin; }

      Tex& tex = m_tex[0];
      TexMediaSample* p = static_cast< TexMediaSample* >(m_pLatestSample);
		if (p->IsLocked()) { //このテクスチャでは初のTake
         m_pTexInputPin->getTexAllocator()->GetRectInfo(&tex.rectInfo);
         //REFERENCE_TIME t1;
         //p->GetTime(&tex.time, &t1);
         p->UnlockAndReplaceTexture(&tex.pTex);
         *update = true;
      } else {
         *update = false;
      }
      //*t     = tex.time;
      *pInfo = tex.rectInfo;
      *ppTex = tex.pTex;
      if (*ppTex != NULL) {
         (*ppTex)->AddRef();
      }
   }
   hr = S_OK;

fin:
   ReleaseMutex(m_mutex);
	return hr;
}

HRESULT TexRenderer::ReleaseTexture_Tex()
{
   m_tex[0].pTex->Release();
	return S_OK;
}

HRESULT TexRenderer::SwapTexture_Tex(IDirect3DTexture9** ppTex, RectInfo* pInfo, bool* update)
{
   IDirect3DTexture9* tmp;
   HRESULT hr = TakeTexture_Tex(&tmp, pInfo, update);
   if (FAILED(hr)) { return hr; }
   if (*update) {
      m_tex[0].pTex = *ppTex;
      *ppTex = tmp;
      // m_tex[0].pTex の参照カウントが一つ加算されている状態にしておく
      m_tex[0].pTex->AddRef();
      (*ppTex)->Release();
   }
   ReleaseTexture_Tex();
   return S_OK;
}

HRESULT TexRenderer::DoRenderSample_Tex(IMediaSample *pMediaSample)
{
   WaitForSingleObjectEx(m_mutex, INFINITE, true);

   {
		if (m_pLatestSample) {
			m_pLatestSample->Release();
		}
		m_pLatestSample = pMediaSample;
		m_pLatestSample->AddRef();
   }

   ReleaseMutex(m_mutex);
   return S_OK;
}

//--------------------------------
HRESULT TexRenderer::TakeTexture_Default(IDirect3DTexture9** ppTex, RectInfo* pInfo, bool* update)
{
   WaitForSingleObjectEx(m_mutex, INFINITE, true);

   *update = m_update;
   m_update = false;
   {
      Tex& tex = m_tex[m_iTake];
      //*t     = tex.time;
      *pInfo = tex.rectInfo;
      *ppTex = tex.pTex;
      if (*ppTex) {
         (*ppTex)->AddRef();
      }
   }

	return S_OK;
}

HRESULT TexRenderer::ReleaseTexture_Default()
{
   m_tex[m_iTake].pTex->Release();
   ReleaseMutex(m_mutex);
	return S_OK;
}

HRESULT TexRenderer::SwapTexture_Default(IDirect3DTexture9** ppTex, RectInfo* pInfo, bool* update)
{
   IDirect3DTexture9* tmp;
   HRESULT hr = TakeTexture_Default(&tmp, pInfo, update);
   if (FAILED(hr)) { return hr; }
   if (*update) {
      m_tex[m_iTake].pTex = *ppTex;
      *ppTex = tmp;
      // m_tex.pTex の参照カウントが一つ加算されている状態にしておく
      m_tex[m_iTake].pTex->AddRef();
      (*ppTex)->Release();
   }
   ReleaseTexture_Default();
   return S_OK;
}

namespace {
HRESULT TransferBitmap2Surface(IDirect3DSurface9* pTex, IMediaSample *pSample, const RectInfo*, bool vflip);
HRESULT TransferBitmap2Texture(IDirect3DTexture9* pTex, IMediaSample *pSample, const RectInfo*, bool vflip);
}

HRESULT TexRenderer::DoRenderSample_Default(IMediaSample *pMediaSample)
{
   HRESULT hr;
   int take0 = m_iTake ^ 1;
   {
      Tex& tex = m_tex[take0];
		m_pTexInputPin->getTexAllocator()->GetRectInfo(&tex.rectInfo);
      hr = TransferBitmap2Texture(tex.pTex, pMediaSample, &tex.rectInfo, true);
      pMediaSample->Release();
	}

   if (! FAILED(hr)) {
      WaitForSingleObjectEx(m_mutex, INFINITE, true);
      m_iTake = take0;
      m_update = true;
      ReleaseMutex(m_mutex);
   }
   return hr;
}

namespace {

HRESULT TransferBitmap2Texture(IDirect3DTexture9* pTex, IMediaSample *pSample, const RectInfo* pRectInfo, bool vflip)
{
	if (pSample == NULL) { return E_FAIL; }
	if (pTex == NULL) { return E_FAIL; }

   IDirect3DSurface9* pSurface;
   HRESULT hr = pTex->GetSurfaceLevel(0, &pSurface);
   if (FAILED(hr)) { return hr; }

   hr = TransferBitmap2Surface(pSurface, pSample, pRectInfo, vflip);
   pSurface->Release();
	return hr;
}

HRESULT TransferBitmap2Surface(IDirect3DSurface9* pSurface, IMediaSample *pSample, const RectInfo* pRectInfo, bool vflip)
{
	if (pSample == NULL) { return E_FAIL; }
	if (pSurface == NULL) { return E_FAIL; }

	BYTE* pSrc;
	pSample->GetPointer(&pSrc);
	if (pSrc == NULL) { return S_FALSE; }

	D3DSURFACE_DESC d;
	pSurface->GetDesc(&d);

	D3DLOCKED_RECT lockRect;   
	HRESULT hr = pSurface->LockRect(&lockRect, NULL, 0);

	if( SUCCEEDED(hr) ) {
		BYTE* pDst;

		long w = pRectInfo->bmi.biWidth;
		long h = pRectInfo->bmi.biHeight;
		long linesize = w * pRectInfo->pFormat->pixelsize;

		if (vflip) {
			for(int i = h-1; 0 <= i; --i) {
				pDst = ((BYTE*)lockRect.pBits) + lockRect.Pitch * i;
				memcpy(pDst, pSrc, linesize);
				pSrc += linesize;
			}
		} else {
			for(int i=0; i<h; ++i) {
				pDst = ((BYTE*)lockRect.pBits) + lockRect.Pitch * i;
				memcpy(pDst, pSrc, linesize);
				pSrc += linesize;
			}
		}
		pSurface->UnlockRect(); //ロック解除   
	}   
	return S_OK;
}
}

}


/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 3
 * End:
 */
