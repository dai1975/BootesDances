#include "TexAllocator.h"
#include <Dvdmedia.h> //BITMAPINFOHEADER2

namespace DirectShowTexture {

//-------------------------------------------------------------------
/** TexAllocator
 *   - TexMediaSample を生成・管理する
 *   - TexRenderer が使う描画バッファ用テクスチャの作成をサポート
 *   - 実装は CMemAllocator を参考に
 *
 * Alloc() でテクスチャを生成し、それを渡して TexMediaSample を作る。
 * テクスチャ生成には幅高さ情報が必要で、Allocator の Property だけでは不十分。
 * これは TexRenderer 接続時のメディアタイプを渡してもらう。
 * ネゴシエーションもこの TexRendererAllocator で行う。
 */
TexAllocator::TexAllocator(
  D3DPOOL pool, TCHAR* pName, LPUNKNOWN pUnk, HRESULT* phr)
: CBaseAllocator(pName, pUnk, phr, TRUE, TRUE)
, m_pDev(NULL)
, m_pool(pool)
, m_pMediaType(NULL)
{
}

#ifdef UNICODE
TexAllocator::TexAllocator(
  D3DPOOL pool, char* pName, LPUNKNOWN pUnk, HRESULT* phr)
: CBaseAllocator(pName, pUnk, phr, TRUE, TRUE)
, m_pDev(NULL)
, m_pool(pool)
, m_pMediaType(NULL)
{
}
#endif

TexAllocator::~TexAllocator()
{
    Decommit();
    ReallyFree();
	 if (m_pMediaType) { delete m_pMediaType; }
}

HRESULT TexAllocator::GetBuffer(__deref_out IMediaSample **ppSample,
                                  __in_opt REFERENCE_TIME *pStartTime,
                                  __in_opt REFERENCE_TIME *pEndTime,
                                  DWORD dwFlags
                                  )
{
	HRESULT hr = CBaseAllocator::GetBuffer(ppSample, pStartTime, pEndTime, dwFlags);
	if (FAILED(hr)) { return hr; }

	if (*ppSample == NULL) { return E_FAIL; }
	TexMediaSample* p = static_cast< TexMediaSample* >(*ppSample);
	return p->LockTexture(&m_rectInfo);
}

HRESULT TexAllocator::ReleaseBuffer(IMediaSample *pSample)
{
	CheckPointer(pSample,E_POINTER);
	ValidateReadPtr(pSample,sizeof(IMediaSample));

	TexMediaSample* p = static_cast< TexMediaSample* >(pSample);
	p->UnlockTexture();
	return CBaseAllocator::ReleaseBuffer(pSample);
}

namespace {

static FrameFormat gFmtTbl[] = {
	{ &MEDIASUBTYPE_ARGB32, D3DFMT_A8R8G8B8, 4 },
	{ &MEDIASUBTYPE_RGB32,  D3DFMT_X8R8G8B8, 4 },
	{ &MEDIASUBTYPE_RGB24,  D3DFMT_R8G8B8,   3 },
	{ &MEDIASUBTYPE_RGB565, D3DFMT_R5G6B5,   2 },
	{ &MEDIASUBTYPE_RGB555, D3DFMT_X1R5G5B5, 2 },
//   { &MEDIASUBTYPE_YUY2,   D3DFMT_YUY2,     2 }, // 4:2:2 Packed
//   { &MEDIASUBTYPE_NV12,   (D3DFORMAT)MAKEFOURCC('N','V','1','2'),     2 }, // 4:2:0 Planer
	{ NULL, D3DFMT_UNKNOWN, 0 },
};

inline const FrameFormat* GetFormatByMedia(const GUID* guid) {
	for (int i = 0; gFmtTbl[i].media; ++i) {
		const FrameFormat& f = gFmtTbl[i];
		if (IsEqualGUID(*f.media, *guid)) {
			return (f.d3d == D3DFMT_UNKNOWN)? NULL: &f;
		}
	}
	return NULL;
}

inline bool ParseMediaType(const CMediaType* pMediaType, VideoInfo* pVideo, RectInfo* pRect, int* formattype)
{
   const GUID* pFormatType = pMediaType->FormatType();
   if (pFormatType == NULL) { return false; }

   if(! IsEqualGUID(*pMediaType->Type(), MEDIATYPE_Video)) {
      return false;
   }
   if (pMediaType->cbFormat < sizeof(VIDEOINFOHEADER)) {
      return false;
   }
   if (pMediaType->pbFormat == NULL) {
      return false;
   }

   const BITMAPINFOHEADER* pBMI = NULL;
	if(IsEqualGUID(*pFormatType, FORMAT_VideoInfo)) {
      *formattype = 1;
      const VIDEOINFOHEADER* p = (const VIDEOINFOHEADER *)pMediaType->pbFormat;
      pBMI = &p->bmiHeader;
      if (pVideo) { pVideo->tpf_100ns = p->AvgTimePerFrame; }
   } else if (IsEqualGUID(*pFormatType, FORMAT_VideoInfo2)) {
      *formattype = 2;
      const VIDEOINFOHEADER2* p = (const VIDEOINFOHEADER2 *)pMediaType->pbFormat;
      pBMI = &p->bmiHeader;
      if (pVideo) { pVideo->tpf_100ns = p->AvgTimePerFrame; }
   } else {
      return false;
   }

   const FrameFormat* pFrameFormat = GetFormatByMedia(pMediaType->Subtype());
   if (pFrameFormat == NULL) { return false; }
   if (pRect) {
      ZeroMemory(pRect, sizeof(RectInfo));
      CopyMemory(&pRect->bmi, pBMI, sizeof(BITMAPINFOHEADER));
      pRect->pFormat = pFrameFormat;
   }
   return true;
}
}

HRESULT TexAllocator::CheckMediaType(IDirect3DDevice9* pDev, const CMediaType* pMediaType) const
{
/*
   if (! CheckFormatType(pMediaType, NULL)) {
      return VFW_E_TYPE_NOT_ACCEPTED;
	}

	if(IsEqualGUID(*pMediaType->Type(), MEDIATYPE_Video)){
		if (pMediaType->cbFormat >= sizeof(VIDEOINFOHEADER)) {
			if (pMediaType->Format() != NULL) {
				VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pMediaType->Format();

				if (GetFormatByMedia(pMediaType->Subtype()) != NULL) {
					return S_OK;
				}
			}
		}
	}
	return S_FALSE;
*/
   int formattype;
   if (! ParseMediaType(pMediaType, NULL, NULL, &formattype)) { return VFW_E_TYPE_NOT_ACCEPTED; }
   return S_OK;
}

HRESULT TexAllocator::SetMediaType(IDirect3DDevice9* pDev, const CMediaType *pMediaType, IDirect3DTexture9** ppTex)
{
	if (0 < m_lAllocated) {
		ReallyFree();
	}

	HRESULT hr;
	D3DCAPS9 caps;
	if (FAILED(hr = pDev->GetDeviceCaps(&caps))) { return hr; }

   int formattype;
   if (! ParseMediaType(pMediaType, &m_videoInfo, &m_rectInfo, &formattype)) { return E_FAIL; }
	if (m_rectInfo.bmi.biHeight < 0) {
		m_rectInfo.bmi.biHeight *= -1;
	}
   m_videoInfo.width  = m_rectInfo.bmi.biWidth;
   m_videoInfo.height = m_rectInfo.bmi.biHeight;
   m_videoInfo.max_time = 0;

	LONG w, h;
	w = m_rectInfo.bmi.biWidth;
	h = m_rectInfo.bmi.biHeight;

	// 2の乗数にしとく
	for (w = 1; w < m_rectInfo.bmi.biWidth;  w <<= 1) {
		if (w < 0) { return E_FAIL; }
	}
	for (h = 1; h < m_rectInfo.bmi.biHeight; h <<= 1) {
		if (h < 0) { return E_FAIL; }
	}

	if (UINT_MAX < w || UINT_MAX < h) { return E_FAIL; }
	UINT uw = (UINT)w;
	UINT uh = (UINT)h;
	if (caps.MaxTextureWidth < uw || caps.MaxTextureHeight < uh) { return E_FAIL; }

	m_rectInfo.texWidth  = uw;
	m_rectInfo.texHeight = uh;
//	m_rectInfo.pFormat = pFormat;
	m_pDev = pDev;

	if (FAILED(hr = CreateTexture(ppTex))) {
		m_pDev = NULL;
		return hr;
	}

	if (m_pMediaType) { delete m_pMediaType; }
	m_pMediaType = CreateMediaType(pMediaType);
	{
		D3DLOCKED_RECT r;
		hr = (*ppTex)->LockRect(0, &r, NULL, 0);
//			D3DLOCK_NOOVERWRITE | D3DLOCK_NOSYSLOCK | D3DLOCK_READONLY);
		(*ppTex)->UnlockRect(0);

		m_rectInfo.texPitch = r.Pitch;

      BITMAPINFOHEADER *pBMI;
      RECT *pSource, *pTarget;
      if (formattype == 1) {
         VIDEOINFOHEADER *p = (VIDEOINFOHEADER *)m_pMediaType->pbFormat;
         pBMI = &p->bmiHeader;
         pSource = &p->rcSource;
         pTarget = &p->rcTarget;
      } else if (formattype == 2) {
         VIDEOINFOHEADER2 *p = (VIDEOINFOHEADER2 *)m_pMediaType->pbFormat;
         pBMI = &p->bmiHeader;
         pSource = &p->rcSource;
         pTarget = &p->rcTarget;
      } else {
         return E_FAIL;
      }
      pBMI->biWidth  = r.Pitch / m_rectInfo.pFormat->pixelsize;
      pBMI->biHeight = -m_rectInfo.bmi.biHeight;

      pSource->top    = 0;
      pSource->bottom = 0;
      pSource->left   = 0;
      pSource->right  = 0;

      pTarget->top    = 0;
      pTarget->bottom = m_rectInfo.bmi.biHeight;
      pTarget->left   = 0;
      pTarget->right  = m_rectInfo.bmi.biWidth;
	}

	return S_OK;
}

HRESULT TexAllocator::CreateTexture(IDirect3DTexture9** pp)
{
	if (m_pDev == NULL) { return E_FAIL; }
   HRESULT hr;
   hr = m_pDev->CreateTexture(
      m_rectInfo.texWidth, m_rectInfo.texHeight, 1, 0,
      m_rectInfo.pFormat->d3d, m_pool, pp, NULL);

   if (FAILED(hr)) {
      if (hr == D3DERR_INVALIDCALL) {
         printf("invalidcall\n");
      } else if (hr == D3DERR_OUTOFVIDEOMEMORY) {
         printf("outofvideomemory\n");
      } else if (hr == E_OUTOFMEMORY) {
         printf("outofmemory\n");
      } else {
         printf("unknown\n");
      }

   } else {
   }
   return hr;
}

/* This sets the size and count of the required samples. The memory isn't
   actually allocated until Commit() is called, if memory has already been
   allocated then assuming no samples are outstanding the user may call us
   to change the buffering, the memory will be released in Commit() */
STDMETHODIMP
TexAllocator::SetProperties(
                __in ALLOCATOR_PROPERTIES* pRequest,
                __out ALLOCATOR_PROPERTIES* pActual)
{
    CheckPointer(pActual,E_POINTER);
    ValidateReadWritePtr(pActual,sizeof(ALLOCATOR_PROPERTIES));
    CAutoLock cObjectLock(this);

    ZeroMemory(pActual, sizeof(ALLOCATOR_PROPERTIES));

    ASSERT(pRequest->cbPrefix == 0);
    ASSERT(pRequest->cbAlign == 1);
    ASSERT(pRequest->cbBuffer > 0);
	 long size = m_rectInfo.bmi.biWidth * m_rectInfo.bmi.biHeight * m_rectInfo.pFormat->pixelsize;
    ASSERT(pRequest->cbBuffer == size);

	 {
		 ALLOCATOR_PROPERTIES prop = *pRequest;
		 if (prop.cBuffers < 2) { prop.cBuffers = 2; }
		 return CBaseAllocator::SetProperties(&prop, pActual);
	 }
}

// override this to allocate our resources when Commit is called.
//
// note that our resources may be already allocated when this is called,
// since we don't free them on Decommit. We will only be called when in
// decommit state with all buffers free.
//
// object locked by caller
HRESULT TexAllocator::Alloc(void)
{
	CAutoLock lck(this);

	/* Check he has called SetProperties */
	HRESULT hr = CBaseAllocator::Alloc();
	if (FAILED(hr)) {
		return hr;
	}

	/* If the requirements haven't changed then don't reallocate */
	if (hr == S_FALSE) {
		ASSERT(m_lAllocated == m_lCount);
		return NOERROR;
	}
	ASSERT(hr == S_OK); // we use this fact in the loop below

	/* Free the old resources */
	if (0 < m_lAllocated) {
		ReallyFree();
	}

	ASSERT(m_lAllocated == 0);

	VIDEOINFOHEADER *pVideoHeader = (VIDEOINFOHEADER *)m_pMediaType->pbFormat;
	long size = pVideoHeader->bmiHeader.biWidth * pVideoHeader->bmiHeader.biHeight;

   TexMediaSample* pSample;
	IDirect3DTexture9* pTex;
	for (; m_lAllocated < m_lCount; ++m_lAllocated) {
		hr = CreateTexture(&pTex);
		if (FAILED(hr)) {
			return E_OUTOFMEMORY;
		}

		pSample = new TexMediaSample(NAME("TexMediaSample"), this, &hr, pTex);
		pTex->Release();
		pTex = NULL;

		ASSERT(SUCCEEDED(hr));
		if (pSample == NULL) {
			return E_OUTOFMEMORY;
		}
		pSample->SetMediaType(m_pMediaType);

		// This CANNOT fail
		m_lFree.Add(pSample);
	}

	m_bChanged = FALSE;
	return NOERROR;
}

// override this to free up any resources we have allocated.
// called from the base class on Decommit when all buffers have been
// returned to the free list.
//
// caller has already locked the object.

// in our case, we keep the memory until we are deleted, so
// we do nothing here. The memory is deleted in the destructor by
// calling ReallyFree()
void TexAllocator::Free(void)
{
    return;
}


// called from the destructor (and from Alloc if changing size/count) to
// actually free up the memory
void TexAllocator::ReallyFree(void)
{
    /* Should never be deleting this unless all buffers are freed */

    ASSERT(m_lAllocated == m_lFree.GetCount());

    /* Free up all the CMediaSamples */

    CMediaSample *pSample;
    for (;;) {
        pSample = m_lFree.RemoveHead();
        if (pSample != NULL) {
            delete pSample;
        } else {
            break;
        }
    }

    m_lAllocated = 0;
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
