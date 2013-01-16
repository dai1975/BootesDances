#include "TexPlayer.h"
#include <stdlib.h>
#include <DvdMedia.h> //VIDEOINFOHEADER2
#include "CharConverter.h"

namespace DirectShowTexture {

namespace {
HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) ;
void RemoveFromRot(DWORD pdwRegister);
HRESULT SaveGraphFile( IGraphBuilder *pGraph , WCHAR *wszPath );
}

HRESULT Player::CreateInstance(IDirect3DDevice9* pDev, D3DPOOL pool, Player** ppPlayer)
{
   TexPlayer* p = new TexPlayer(pDev, pool);
   *ppPlayer = p;
   return S_OK;
}

Player::Player() { }
Player::~Player() { }

TexPlayer::TexPlayer(IDirect3DDevice9* pDev, D3DPOOL pool)
: Player()
{
   m_pDev = pDev;
   m_pDev->AddRef();
   m_pool = pool;
   m_pControl = NULL;
   m_pSeeking = NULL;
   m_pRenderer = NULL;
   m_pGraph = NULL;
}

TexPlayer::~TexPlayer()
{
   if (m_pControl) { m_pControl->Release(); }
   if (m_pSeeking) { m_pSeeking->Release(); }
   if (m_pRenderer) { m_pRenderer->Release(); }
   if (m_pGraph) { m_pGraph->Release(); }
   if (m_pDev) { m_pDev->Release(); }
}

HRESULT TexPlayer::TakeTexture(IDirect3DTexture9** ppTex, RectInfo* pRect, bool* update)
{
   if (m_pRenderer == NULL) {
      return E_FAIL;
   }
   return m_pRenderer->TakeTexture(ppTex, pRect, update);
}
HRESULT TexPlayer::ReleaseTexture()
{
   if (m_pRenderer == NULL) {
      return E_FAIL;
   }
   return m_pRenderer->ReleaseTexture();
}
HRESULT TexPlayer::CreateTexture(IDirect3DTexture9** ppTex, RectInfo* pRect)
{
   if (m_pRenderer == NULL) {
      return E_FAIL;
   }
   return m_pRenderer->CreateTexture(ppTex, pRect);
}

HRESULT TexPlayer::SwapTexture(IDirect3DTexture9** ppTex, RectInfo* pRect, bool* update)
{
   if (m_pRenderer == NULL) {
      return E_FAIL;
   }
   return m_pRenderer->SwapTexture(ppTex, pRect, update);
}


void TexPlayer::clear()
{
   if( m_pControl != NULL ) {
      OAFilterState state;
      do {
         m_pControl->Stop();
         m_pControl->GetState(0, & state );
      } while( state != State_Stopped ) ;
   }

   if (m_pRenderer) {
      m_pRenderer->Release();
      m_pRenderer = NULL;
   }
   if (m_pSeeking) {
      m_pSeeking->Release();
      m_pSeeking = NULL;
   }
   if (m_pControl) {
      m_pControl->Release();
      m_pControl = NULL;
   }

   if (m_pGraph) {
      RemoveFromRot(m_rot);
      m_pGraph->Release();
      m_pGraph = NULL;
   }
}

namespace {

class FilterInfo {
public:
   char *name, *vender;
   CLSID clsid;

   FilterInfo(): name(NULL), vender(NULL) { }
   ~FilterInfo() {
      if (name) { free(name); }
      if (vender) { free(vender); }
   }

   bool probe(IBaseFilter* p) {
      if (name) { free(name); name = NULL; }
      if (vender) { free(vender); vender = NULL; }

      HRESULT hr;

      FILTER_INFO info;
      if (FAILED(hr = p->QueryFilterInfo(&info))) { return false; }
      name = CharConverter::W2C(info.achName);
      if (info.pGraph) { info.pGraph->Release(); }

      LPWSTR wstr;
      if (!FAILED(hr = p->QueryVendorInfo(&wstr))) { //E_NOTIMPL
         vender = CharConverter::W2C(wstr);
         CoTaskMemFree(wstr);
      }

      hr = p->GetClassID(&clsid);
      
      return (name != NULL && vender != NULL);
   }
};
/*
class Pins {
public:
   IPin* pStreamIn;
   IPin* pVideoIn;
   IPin* pVideoOut;
   IPin* pAudioOut;
   IPin* pStreamOut;

   Pins(): pStreamIn(NULL), pVideoIn(NULL), pVideoOut(NULL), pAudioOut(NULL), pStreamOut(NULL) { }
   ‾Pins() {
      if (pStreamIn) { pStreamIn->Release(); pStreamIn = NULL; }
      if (pVideoIn) { pVideoIn->Release(); pVideoIn = NULL; }
      if (pVideoOut) { pVideoOut->Release(); pVideoOut = NULL; }
      if (pAudioOut) { pAudioOut->Release(); pAudioOut = NULL; }
      if (pStreamOut) { pStreamOut->Release(); pStreamOut = NULL; }
   }

   HRESULT probe(IGraphBuilder* pGraph, IBaseFilter* pFilter, TexRenderer* pRenderer) {
      HRESULT hr;
      IEnumPins* pPins = NULL;
      if (FAILED(hr = pFilter->EnumPins(&pPins))) { return hr; }

      for (IPin* pPin; pPins->Next(1, &pPin, NULL) == S_OK; pPin->Release()) {
         PIN_DIRECTION dir;
         pPin->QueryDirection(&dir);

         if (dir == PINDIR_INPUT) {
            if (pStreamIn == NULL) {
            }

            if (pGraph->Connect(pStreamOut, pPin) == S_OK) {
               in = true;
            }
         }
         if ( (!out) && (dir == PINDIR_OUTPUT)) {
            if (pGraph->Connect(pPin, pRenderer->GetTexInputPin()) == S_OK) {
               out = true;
            }
         }
         if (in && out) { break; }
      }
      pPins->Release();

      if (in && out) {
         hr = S_OK;
      } else {
         pGraph->RemoveFilter(pSplitter);
         hr = E_FAIL;
      }
   }
};
*/
HRESULT Connect(IGraphBuilder* pGraph, IPin* pSrcOut, IBaseFilter* pSplitter, TexRenderer* pRenderer)
{
   HRESULT hr;
   FilterInfo info;
   info.probe(pSplitter);
/*
   IPin* pIn = NULL;
   IPin* pVOut = NULL;
   IPin* pAOut = NULL;
   {
      IEnumPins* pPins;
      if (FAILED(hr = pSplitter->EnumPins(&pPins))) { goto fin; }

      for (IPin* pPin; pPins->Next(1, &pPin, NULL) == S_OK; pPin->Release()) {
         PIN_DIRECTION dir;
         pPin->QueryDirection(&dir);

         if (dir == PINDIR_INPUT) {
            if (pIn == NULL) {
pIn->
            }
         }
         if ( (!out) && (dir == PINDIR_OUTPUT)) {
            if (pGraph->Connect(pPin, pRenderer->GetTexInputPin()) == S_OK) {
               out = true;
            }
         }
         if (in && out) { break; }
      }
      pPins->Release();
   }
 */
   {
      IEnumPins* pPins;
      if (FAILED(hr = pSplitter->EnumPins(&pPins))) { goto fin; }

      bool in  = false;
      bool out = false;
      for (IPin* pPin; pPins->Next(1, &pPin, NULL) == S_OK; pPin->Release()) {
         PIN_DIRECTION dir;
         pPin->QueryDirection(&dir);

         if ( (!in) && (dir == PINDIR_INPUT)) {
            if (pGraph->Connect(pSrcOut, pPin) == S_OK) {
               in = true;
            }
         }
         if ( (!out) && (dir == PINDIR_OUTPUT)) {
            if (pGraph->Connect(pPin, pRenderer->GetTexInputPin()) == S_OK) {
               out = true;
            }
         }
         if (in && out) { break; }
      }
      pPins->Release();

      if (in && out) {
         hr = S_OK;
      } else {
         pGraph->RemoveFilter(pSplitter);
         hr = E_FAIL;
      }
   }

fin:
   return hr;
}

HRESULT ConnectAudio(IGraphBuilder* pGraph)
{
   HRESULT hr;
   IEnumFilters* pEnum = NULL;
   hr = pGraph->EnumFilters(&pEnum);
   if (FAILED(hr)) { goto fin; }

   IBaseFilter* pFilter = NULL;
   while (S_OK == pEnum->Next(1, &pFilter, 0)) {
      pFilter->Release();
   }
   pEnum->Release();
   hr = S_OK;

fin:
   return hr;
}

HRESULT _Connect(IGraphBuilder* pGraph, IBaseFilter* pSrc, TexRenderer* pRenderer)
{
   HRESULT hr = S_OK;

   FilterInfo info;
   info.probe(pSrc);

   IFilterMapper2 *pMapper = NULL;
   IEnumMoniker *pEnum = NULL;

   IPin* pVideoOut = NULL;
   IPin* pAudioOut = NULL;
   IPin* pStreamOut = NULL;
   GUID tVideoOut[2];
   GUID tAudioOut[2];
   GUID tStreamOut[2];

   { //ソースフィルタの出力ピンを取得しておく
      IEnumPins* pPins;
      if (FAILED(hr = pSrc->EnumPins(&pPins))) { goto fin; }

      for (IPin* pPin; pPins->Next(1, &pPin, NULL) == S_OK; pPin->Release()) {
         PIN_DIRECTION dir;
         pPin->QueryDirection(&dir);
         if (dir != PINDIR_OUTPUT) { continue; }

         IEnumMediaTypes* pEnum = NULL;
         if (FAILED(pPin->EnumMediaTypes(&pEnum))) { continue; }

         AM_MEDIA_TYPE *pAM = NULL;
         while (pEnum->Next(1, &pAM, NULL) == S_OK) {
            if(pVideoOut == NULL && IsEqualGUID(pAM->majortype, MEDIATYPE_Video)) {
               pVideoOut = pPin;
               tVideoOut[0] = pAM->majortype;
               tVideoOut[1] = pAM->subtype;
            } else if (pAudioOut == NULL && IsEqualGUID(pAM->majortype, MEDIATYPE_Audio)) {
               pAudioOut = pPin;
               tAudioOut[0] = pAM->majortype;
               tAudioOut[1] = pAM->subtype;
            } else if (pStreamOut == NULL && IsEqualGUID(pAM->majortype, MEDIATYPE_Stream)) {
               pStreamOut = pPin;
               tStreamOut[0] = pAM->majortype;
               tStreamOut[1] = pAM->subtype;
            }
            DeleteMediaType(pAM);
         }
         pEnum->Release();
      }
      pPins->Release();

      if (pVideoOut == NULL && pStreamOut == NULL) { hr = E_FAIL; goto fin; }
      /*
      IFileSourceFilter* pFile = NULL;
      LPOLESTR tmp;
      AM_MEDIA_TYPE am;

      if (FAILED(hr = pSrc->QueryInterface(&pFile))) { goto fin; }

      ZeroMemory(&am, sizeof(am));
      hr = pFile->GetCurFile(&tmp, &am);
      pFile->Release();
      if (FAILED(hr)) { return hr; }

      intype[0] = am.majortype;
      intype[1] = am.subtype;
      CoTaskMemFree(tmp);
      FreeMediaType(am);
      */
   }
   if (pVideoOut) {
      pGraph->Connect(pVideoOut, pRenderer->GetTexInputPin());
   } else if (pStreamOut) {
      pGraph->Connect(pStreamOut, pRenderer->GetTexInputPin());
   }

#if 0
   // スプリッタを挟んで連結
   {
      hr = CoCreateInstance(CLSID_FilterMapper2, 
         NULL, CLSCTX_INPROC, IID_IFilterMapper2, 
         (void **) &pMapper);
      if (FAILED(hr)) { goto fin; }

      /* 複数の出力ピンを持つフィルタの検索指定ではない？
      GUID outtype[4] = {
         MEDIATYPE_Video, GUID_NULL,
         MEDIATYPE_Audio, GUID_NULL, //MEDIASUBTYPE_None
      };
      */
      GUID outtype[2] = { MEDIATYPE_Video, GUID_NULL, };
      pMapper->EnumMatchingFilters(&pEnum, 0, false, MERIT_DO_NOT_USE+1,
         1, 1, intype, NULL, NULL,
         false, true, 1, outtype, NULL, NULL);
      if (FAILED(hr)) { goto fin; }

      IMoniker *pMon;
      ULONG cFetched;
      while (pEnum->Next(1, &pMon, &cFetched) == S_OK) {
         IBaseFilter* pFilter = NULL;
         if (FAILED(pMon->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter))) {
            continue;
         }
         hr = Connect(pGraph, pSrcOut, pFilter, pRenderer);
         pFilter->Release();
         pMon->Release();
         if (! FAILED(hr)) { goto fin; } //S_OK
      }
      hr = E_FAIL;
   }
#endif

fin:
   if (pEnum) { pEnum->Release(); }
   if (pMapper) { pMapper->Release(); }
   if (pVideoOut != NULL) { pVideoOut->Release(); }
   if (pAudioOut != NULL) { pAudioOut->Release(); }
   if (pStreamOut != NULL) { pStreamOut->Release(); }
   return hr;
}

HRESULT Connect(IGraphBuilder* pGraph, IBaseFilter* pSrc, TexRenderer* pRenderer)
{
   HRESULT hr = S_OK;
   ICaptureGraphBuilder2 *pCGB2 = NULL;

   FilterInfo info;
   info.probe(pSrc);

   hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER,IID_ICaptureGraphBuilder2,(void**)&pCGB2);
   if (FAILED(hr)) { goto fin; }

   if (FAILED(hr = pCGB2->SetFiltergraph(pGraph))) { goto fin; }
   // BUG: この RenderStream から TexInputPin#GetAllocator() に至るが、そこで SEGV することがある
   if (FAILED(hr = pCGB2->RenderStream(0, 0, pSrc, 0, pRenderer))) { goto fin; }
   if (FAILED(hr = pCGB2->RenderStream(0, &MEDIATYPE_Audio, pSrc, 0, 0))) { goto fin; }

fin:
   if (pCGB2) { pCGB2->Release(); }
   return hr;
}

}

bool TexPlayer::unload()
{
   clear();
   return true;
}

bool TexPlayer::load(const TCHAR* fname_tc, VideoInfo* pInfo)
{
   wchar_t* fname = CharConverter::T2W(fname_tc);
   bool b = loadW(fname, pInfo);
   free(fname);
   return b;
}

bool TexPlayer::loadW(const wchar_t* fname, VideoInfo* pInfo)
{
   // Clear DirectShow interfaces (COM smart pointers)
   clear();

   HRESULT hr;

   { // DirectShow
      hr = CoCreateInstance(CLSID_FilterGraph, 0, 
         CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pGraph));
      if (hr != S_OK || m_pGraph == NULL) { return false; }

      hr = AddToRot(m_pGraph, &m_rot);

		m_pRenderer = new TexRenderer(m_pDev, m_pool, &hr);
      if (hr != S_OK || m_pRenderer == NULL) { return false; }
		m_pRenderer->AddRef();

      hr = m_pGraph->AddFilter(m_pRenderer, _T("TexRenderer"));
      if (hr != S_OK) { return false; }

      hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&m_pControl));
      if (hr != S_OK) { return false; }

      hr = m_pGraph->QueryInterface(IID_PPV_ARGS(&m_pSeeking));
      if (hr != S_OK) { return false; }
   }

   { // グラフの構築とソース情報の取得
/*
      hr = m_pGraph->RenderFile(fname, NULL);
      if (hr != S_OK) { return false; }
*/
      IBaseFilter* pSrc = NULL;
      do {
         hr = m_pGraph->AddSourceFilter(fname, fname, &pSrc);
         if (FAILED(hr)) { break; }

         hr = Connect(m_pGraph, pSrc, m_pRenderer);
         if (FAILED(hr)) { break; }
      } while(0);
      if (pSrc) { pSrc->Release(); }
      if (FAILED(hr)) { return false; }

      SaveGraphFile(m_pGraph, L"d:¥¥tmp¥¥graph.txt");
   }
   while (m_pRenderer->GetTexInputPin()->IsFlushing()) {
      Sleep(10);
   }

   m_pRenderer->GetInitialVideoInfo(pInfo);

   // 現環境で TIME_FORMAT の FRAME が使えないので MEDIA_TIME で
   if (FAILED(m_pSeeking->GetAvailable(&m_begin, &m_end))) {
      return false;
   }
//   m_pControl->Run();
   seek_set(0);
   pInfo->max_time = m_end - m_begin;

   return true;
}

bool TexPlayer::play()
{
   if (m_pRenderer->GetTexInputPin()->IsFlushing()) {
      DbgBreak("flushing");
   }

   m_pControl->Run();
   return true;
}

bool TexPlayer::pause()
{
   //m_pControl->Stop();
   //seek_cur(0);
   m_pControl->Pause();
   return true;
}

bool TexPlayer::seek0(LONGLONG pos)
{
   if (pos < m_begin) {
      pos = m_begin;
   } else if (m_end < pos) {
      pos = m_end;
   }

   DWORD flags[2] = {
      AM_SEEKING_AbsolutePositioning,
      AM_SEEKING_NoPositioning
   };

   m_pControl->Pause();
   m_pSeeking->SetPositions(&pos, flags[0], NULL, flags[1]);
//   m_pControl->StopWhenReady();
   return true;
}

bool TexPlayer::seek_set(__int64 offset)
{
   LONGLONG pos = m_begin + offset;
   return seek0(pos);
}

bool TexPlayer::seek_cur(__int64 offset)
{
   m_pControl->Pause();
   LONGLONG pos;
   {
      LONGLONG cur;
      HRESULT hr = m_pSeeking->GetCurrentPosition(&cur);
      if (FAILED(hr)) {
         cur = 0;
      }
      pos = offset + cur;
   }
   return seek0(pos);
}

bool TexPlayer::seek_end(__int64 offset)
{
   LONGLONG pos = m_end + offset;
   return seek0(pos);
}

LONGLONG TexPlayer::getTime()
{
   if (m_pSeeking == NULL) {
      return -1;
   }
   LONGLONG t;
   if (FAILED(m_pSeeking->GetCurrentPosition(&t))) {
      return -2;
   }
   return t;
}

DirectShowTexture::Player::STATE TexPlayer::getState()
{
   if (m_pControl == NULL) {
      return DirectShowTexture::Player::S_UNLOAD;
   }

   OAFilterState s;
   if (FAILED(m_pControl->GetState(0, &s))) {
      return DirectShowTexture::Player::S_ERR;
   }

   switch (s)
   {

   case State_Stopped: return DirectShowTexture::Player::S_PAUSE;
   case State_Paused: return DirectShowTexture::Player::S_PAUSE;
   case State_Running: return DirectShowTexture::Player::S_RUN;
   }
   return DirectShowTexture::Player::S_ERR;
}

namespace {
HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
    IMoniker * pMoniker;
    IRunningObjectTable *pROT;
    if (FAILED(GetRunningObjectTable(0, &pROT))) {
        return E_FAIL;
    }
    WCHAR wsz[256];
    wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) {
        hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph,
            pMoniker, pdwRegister);
        pMoniker->Release();
    }
    pROT->Release();
    return hr;
}
void RemoveFromRot(DWORD pdwRegister)
{
    IRunningObjectTable *pROT;
    if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
        pROT->Revoke(pdwRegister);
        pROT->Release();
    }
}

HRESULT SaveGraphFile( IGraphBuilder *pGraph , WCHAR *wszPath )
{
   HRESULT hr;
   IEnumFilters* pEnum;
   IBaseFilter* pFilter;

//   HANDLE fh = CreateFile(wszPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   HANDLE fh = CreateFile(wszPath, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
   if (fh == INVALID_HANDLE_VALUE) { return E_FAIL; }
   SetFilePointer(fh, 0, 0, FILE_END);

   {
      hr = pGraph->EnumFilters(&pEnum);
      if (FAILED(hr)) { goto fin; }

//      char mbs[1024];
      while (S_OK == pEnum->Next(1, &pFilter, 0)) {
         FilterInfo info;
         DWORD olen;
         if (info.probe(pFilter)) {
            WriteFile(fh, info.name, strlen(info.name), &olen, NULL); 
            WriteFile(fh, "\n", 2, &olen, NULL); 
         }
         pFilter->Release();
      }
      pEnum->Release();
   }
   hr = S_OK;

fin:
   CloseHandle(fh);
   return hr;
}

HRESULT _SaveGraphFile( IGraphBuilder *pGraph , WCHAR *wszPath )
{
    const WCHAR wszStreamName[] = L"ActiveMovieGraph";
    HRESULT hr;
    IStorage *pStorage = NULL;

    // まず，GRFファイルを保持するドキュメントファイルを作る
    hr = StgCreateDocfile( wszPath , STGM_CREATE | STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE , 0 , &pStorage );
    if( FAILED( hr ) ){
        return hr;
    }

    // 次に，保存するストリームを作る
    IStream *pStream;
    hr = pStorage->CreateStream( wszStreamName , STGM_WRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE , 0 , 0 , &pStream );
    if( FAILED( hr ) ){
        return hr;
    }

    // IPersistStream::Saveメソッドはストリームを存続できる形に変換する
    IPersistStream *pPersist = NULL;
    pGraph->QueryInterface( IID_IPersistStream , reinterpret_cast<void **>(&pPersist) );
    hr = pPersist->Save( pStream , TRUE );
    pStream->Release();
    pPersist->Release();
    if( SUCCEEDED( hr ) ){
        hr = pStorage->Commit( STGC_DEFAULT );
    }
    pStorage->Release();
    return hr;
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

