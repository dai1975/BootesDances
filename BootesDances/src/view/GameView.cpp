#include "GameView.h"
//#include <bootes/lib/d3dx9/Font.h>
#include <bootes/lib/util/Timer.h>
#include <math.h>

GameView::GameView(StageManagerProxy* mng, D3DPOOL pool)
{
/*
   _pRenderSurface = NULL;
   _pDepthSurface = NULL;
   _pExportTmpSurface = NULL;
   _pExportTexture = NULL;
*/
   _pool = pool;

   _pMoviePlayer    = mng->getMoviePlayer();
   _pMovePresenter  = mng->getMovePresenterImpl();
   _pWiimoteHandler = mng->getWiimoteHandler();
   _pStageManager   = mng;
   _enabled = true;
}

GameView::~GameView()
{
//   clearBuffer();
}

Scene GameView::getScene(bool requireTexture) const
{
   Scene scene = _pMoviePlayer->getScene(false);
   IDirect3DTexture9* pTex = _texView.refTexture();
   if (requireTexture && pTex) { //_pExportTexture) {
      //_pExportTexture->AddRef();
      //scene.texture() = _pExportTexture;
      scene.texture() = pTex;
   }
   return scene;
}
StageClock GameView::getStageClock() const
{
   return _pMoviePlayer->getStageClock();
}
VideoInfo  GameView::getVideoInfo() const
{
   return _pMoviePlayer->getVideoInfo();
}

void GameView::onResetDevice()
{
   IDirect3DDevice9* pDev = g_pFnd->getD3D9Device();
//   clearBuffer();
   _texView.onResetDevice();
   _pMoviePlayer->onResetDevice();
   _pMovePresenter->onResetDevice();
}

void GameView::onLostDevice()
{
   //clearBuffer();
   _texView.onLostDevice();
   _pMoviePlayer->onLostDevice();
   _pMovePresenter->onLostDevice();
}

bool GameView::onInput(const ::bootes::lib::framework::InputEvent* ev)
{
   switch (ev->_type) {
   case ::bootes::lib::framework::InputEvent::T_WNDMSG:
      return false;

   case ::bootes::lib::framework::InputEvent::T_WIIMOTE:
      {
         const ::bootes::lib::framework::WiimoteEvent* wev = static_cast< const ::bootes::lib::framework::WiimoteEvent* >(ev);
         Scene scene = _pMoviePlayer->getScene(false);
         if (_pWiimoteHandler != NULL && scene.isValid()) {
            _pWiimoteHandler->handleWiimote(&scene, wev);
/*
            if (wev->isHeld1sec(::bootes::lib::framework::WiimoteEvent::BTN_A)) {
               _pWiimoteHandler->teach(&scene, wev);
            } else {
               _pWiimoteHandler->test(&scene, wev);
            }
*/
         }
      }
      return false;

   case ::bootes::lib::framework::InputEvent::T_KINECT:
      return false;
   }
   return false;
}

void GameView::onUpdate(double currentTime, int elapsedTime)
{
   switch (_state) {
   case S_0:
      _state = S_RUN;
      break;
   case S_RUN:
      break;
   }
}
/*
void GameView::clearBuffer()
{
#define CLEAR(ptr) if (ptr) { ptr->Release(); ptr = NULL; }

   CLEAR(_pRenderSurface);
   CLEAR(_pDepthSurface);
   CLEAR(_pExportTexture);
   CLEAR(_pExportTmpSurface);

#undef CLEAR
}

HRESULT GameView::createBuffer(IDirect3DTexture9* pTex)
{
   if (_pExportTexture != NULL) { return S_OK; }
   clearBuffer();

   IDirect3DDevice9* pDev = g_pFnd->getD3D9Device();
   HRESULT hr;

   D3DSURFACE_DESC d;
   pTex->GetLevelDesc(0, &d);

   //CreateRenderTarget creates D3DPOOL_DEFAULT surface
   hr = pDev->CreateRenderTarget(
      d.Width, d.Height, d.Format, D3DMULTISAMPLE_NONE, 0, 
      true, &_pRenderSurface, NULL);
   if (FAILED(hr)) { goto fail; }
   _pRenderSurface->GetDesc(&d);

   // 深度ステンシルバッファの作成
   hr = pDev->CreateDepthStencilSurface(
      d.Width, d.Height, D3DFMT_D16,
      D3DMULTISAMPLE_NONE, 0, false, &_pDepthSurface, NULL);
   if (FAILED(hr)) { goto fail; }

   // CeguiTextureImage 提供用テクスチャの作成
   hr = pDev->CreateTexture(
      d.Width, d.Height, 1, 0, d.Format,
      _pool, &_pExportTexture, NULL);
   if (FAILED(hr)) { goto fail; }

   // create temporary texture to copy DEFAULT -> MANAGED
   // there is no function to transfer DEFAULT -> MANAGED and memcpy is very slow.
   if (_pool == D3DPOOL_MANAGED) {
      hr = pDev->CreateOffscreenPlainSurface(
                   d.Width, d.Height, d.Format,
                   D3DPOOL_SYSTEMMEM, &_pExportTmpSurface, NULL);
      if (FAILED(hr)) { goto fail; }
   }

   return S_OK;

fail:
   clearBuffer();
   return hr;
}
*/

void GameView::drawFps(IDirect3DDevice9* pDev, const Scene* scene, int currentTime, int elapsedTime)
{
   float fps = 1000.0f / elapsedTime;
   if (fps > 1000) { fps = 999; }
   char buf[7] = {0};
   sprintf_s(buf, 7, "%6.2f", fps);

   _label.setCharSize(6);
   _label.setText(buf);
   float w = 16*6;
   float h = 32;
   _label.setArea(scene->videoinfo().width - 10 - w, 10, w, h);
   _label.draw();
}

void GameView::onRender(double currentTime, int elapsedTime)
{
   _pMoviePlayer->onRender(currentTime, elapsedTime);

   HRESULT hr;
   IDirect3DDevice9* pDev = g_pFnd->getD3D9Device();
   if (pDev == NULL) { return; }

   if (! _pStageManager->isEnabled()) { return; }

   Scene scene = _pMoviePlayer->getScene(true);
   if (scene.texture() == NULL) { return; }

   pDev->EndScene();
   onRender0(pDev, &scene, currentTime, elapsedTime);
   pDev->BeginScene();
}

void GameView::onRender0(IDirect3DDevice9* pDev, const Scene* scene, double currentTime, int elapsedTime)
{
   HRESULT hr;
   ::bootes::lib::util::Timer timer;
   int dt[10];
   int di = 0;

   int scene_w = scene->videoinfo().width;
   int scene_h = scene->videoinfo().height;

   if (! _texView.isInit()) {
      IDirect3DTexture9* pTex = scene->norefTexture();
      if (pTex == NULL) { return; }

      D3DSURFACE_DESC d;
      pTex->GetLevelDesc(0, &d);
      _texView.init(_pool, d.Width, d.Height, d.Format);
   }
/*
   if (_pExportTexture == NULL) {
      hr = createBuffer(scene->norefTexture());
      if (FAILED(hr)) {
         return;
      }
   }
*/

   IDirect3DSurface9* pBufSurface;
   if (! _texView.beginScene(pDev, &pBufSurface)) { return; }
/*
   // 現在のレンダリングターゲットへの参照を退避
   IDirect3DSurface9 *pBuf0, *pDepth0;
   timer.start();
   {
      hr = pDev->GetRenderTarget(0, &pBuf0);
      if (FAILED(hr)) { return; }
      hr = pDev->GetDepthStencilSurface(&pDepth0);
      if (FAILED(hr)) { pBuf0->Release(); return; }
      { //レンダリングターゲットの切り替え
         hr = pDev->SetRenderTarget(0, _pRenderSurface);
         hr = pDev->SetDepthStencilSurface(_pDepthSurface);
      
      }
   }
   timer.get(NULL, &dt[di++]);

   pDev->BeginScene();
*/

   { //レンダリング
      //IDirect3DSurface9* pBufSurface;
      //これだと直接画面に描画しちゃうみたい
      //hr = pDev->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&psBuf);
      //pBufSurface = _pRenderSurface;
      //pBufSurface->AddRef();

      //動画イメージをレンダリングバッファ(D3DPOOL_DEFAULT)へコピー
      timer.start();
      {
         IDirect3DSurface9* pSceneSurface;
         D3DSURFACE_DESC bufDesc, sceneDesc;

         scene->norefTexture()->GetSurfaceLevel(0, &pSceneSurface);
         //_pRenderSurface->GetDesc(&bufDesc);
         pBufSurface->GetDesc(&bufDesc);
         pSceneSurface->GetDesc(&sceneDesc);

         switch (_pMoviePlayer->getPool()) {
         case D3DPOOL_DEFAULT:
            //hr = pDev->StretchRect(pSceneSurface, NULL, _pRenderSurface, NULL, D3DTEXF_NONE);
            hr = pDev->StretchRect(pSceneSurface, NULL, pBufSurface, NULL, D3DTEXF_NONE);
            break;
         case D3DPOOL_SYSTEMMEM:
            //hr = pDev->UpdateSurface(pSceneSurface, NULL, _pRenderSurface, NULL);
            hr = pDev->UpdateSurface(pSceneSurface, NULL, pBufSurface, NULL);
            break;
         default:
            break;
         }
         pSceneSurface->Release();
      }
      timer.get(NULL, &dt[di++]);

      timer.start();
      _pMovePresenter->onRender(scene, currentTime, elapsedTime);
      timer.get(NULL, &dt[di++]);

      timer.start();
      drawFps(pDev, scene, currentTime, elapsedTime);
      timer.get(NULL, &dt[di++]);

      //pBufSurface->Release();
   }

   pBufSurface->Release();
   _texView.endScene(pDev);
/*
   pDev->EndScene();

   timer.start();
   // レンダリングターゲットの復帰
   {
      hr = pDev->SetRenderTarget(0, pBuf0);
      hr = pDev->SetDepthStencilSurface(pDepth0);
      pBuf0->Release(); //Get時のカウンタをデクリメント
      pDepth0->Release();
   }
   timer.get(NULL, &dt[di++]);

   IDirect3DSurface9* pBufSurface = _pRenderSurface;

   // copy image from rendered surface to export surface
   timer.start();
   if (_pool == D3DPOOL_MANAGED) {
      // because d3d cannot directly copy from DEFAULT to MANAGED,
      //   1. setup temporary surface which is on SYSTEMMEM
      //   2. copy from DEFAULT to SYSTEMMEM by GetRenderTargetData()
      //   3. copy from SYSTEMMEM to MANAGED by memcpy //this is a bit slow.
      do {
         ::bootes::lib::util::Timer timer2; int dt2[10]; int di2 = 0;
         D3DSURFACE_DESC d0, d1;

         timer2.start();
         hr = pDev->GetRenderTargetData(pBufSurface, _pExportTmpSurface);
         timer2.get(NULL, &dt2[di2++]);

         _pExportTmpSurface->GetDesc(&d0);
         _pExportTexture->GetLevelDesc(0, &d1);
         if (d0.Format != d1.Format) { break; }
         if (d0.Width  < scene_w) { break; }
         if (d0.Height < scene_h) { break; }
         if (d1.Width  < scene_w) { break; }
         if (d1.Height < scene_h) { break; }

         D3DLOCKED_RECT r0, r1;
         _pExportTmpSurface->LockRect(&r0, NULL, D3DLOCK_READONLY);
         _pExportTexture->LockRect(0, &r1, NULL, 0);
         {
            int u = r0.Pitch / d0.Width;
            int xbytes = u * d0.Width;
            BYTE* p0 = (BYTE*)r0.pBits;
            BYTE* p1 = (BYTE*)r1.pBits;
            timer2.start();
            for (LONG y=0; y<scene_h; ++y) {
               ::memcpy(p1, p0, xbytes);
               p0 += r0.Pitch;
               p1 += r1.Pitch;
            }
            timer2.get(NULL, &dt2[di2++]);
         }
         _pExportTexture->UnlockRect(0);
         _pExportTmpSurface->UnlockRect();
         timer2.reset();
      } while (0);

   } else if (_pool == D3DPOOL_SYSTEMMEM) {
      IDirect3DSurface9* pDst;
      _pExportTexture->GetSurfaceLevel(0, &pDst);
      hr = pDev->GetRenderTargetData(pBufSurface, pDst);
      pDst->Release();
   }
*/
   timer.get(NULL, &dt[di++]);
}

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
