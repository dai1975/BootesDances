#include "RootViewPlay.h"
#include <bootes/lib/util/Timer.h>

#include <bootes/dances/EvStage.h>
#include <bootes/dances/EvMovie.h>

RootViewPlay::RootViewPlay(const TCHAR* dir)
{
   _pProxy = new StageManagerProxy();
   _pProxy->init(dir, false, D3DPOOL_SYSTEMMEM);

   _pGameView    = new GameView(_pProxy, D3DPOOL_SYSTEMMEM);

   g_pFnd->getEventManager()->subscribe< EvLoadStageResult >(this);

   _pProxy->start();
}

RootViewPlay::~RootViewPlay()
{
   delete _pGameView;
   delete _pProxy;
}

void RootViewPlay::onSubscribe(::bootes::lib::framework::EventManager*)
{
}
void RootViewPlay::onUnsubscribe(::bootes::lib::framework::EventManager*)
{
}

void RootViewPlay::onEvent(const ::bootes::lib::framework::GameTime* gt, const ::bootes::lib::framework::Event* p)
{
   if (p->getEventId() == EvLoadStageResult::GetEventId()) {
      EvPlayMovie e;
      g_pFnd->queue(&e);
   }
}

void RootViewPlay::onLostDevice()
{
   _pGameView->onLostDevice();
}

void RootViewPlay::onResetDevice()
{
   _pGameView->onResetDevice();
}

void RootViewPlay::onUpdate(const ::bootes::lib::framework::GameTime* gt)
{
   _pGameView->onUpdate(gt);
}

void RootViewPlay::onRender(const ::bootes::lib::framework::GameTime* gt)
{
   HRESULT hr;
   ::bootes::lib::util::Timer timer;
   int dt[10];
   int ti = 0;

   IDirect3DDevice9* pDev = g_pFnd->getD3D9Device();
   if (pDev == NULL) { return; }
   hr = pDev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1, 0  );

   timer.start();
   _pGameView->onRender(gt);
   timer.get(NULL, &dt[ti++]);

   timer.start();
   do {
      //IDirect3DTexture9* pTex = scene.refTexture(); //test
      Scene scene = _pGameView->getScene(true);
      IDirect3DTexture9* pTex = scene.refTexture();
      if (pTex == NULL) { break; }
      IDirect3DSurface9* pSrc;
      pTex->GetSurfaceLevel(0, &pSrc);
      
      IDirect3DSurface9* pDst;
      hr = pDev->GetRenderTarget(0, &pDst);

      //hr = pDev->StretchRect(pSrc, NULL, pDst, NULL, D3DTEXF_NONE); //D3DPOOL_DEFUALT
      hr = pDev->UpdateSurface(pSrc, NULL, pDst, NULL); //D3DPOOL_SYSTEM
      if (hr == D3DERR_INVALIDCALL) {
         DbgBreak("");
      }
      pDst->Release();
      pSrc->Release();
      pTex->Release();
   } while(0);
   pDev->EndScene();
   /*
   do {
      ID3DXLine* pLine;
      D3DXCreateLine(pDev, &pLine);
      D3DXVECTOR2 points[] = {
         D3DXVECTOR2(0,0), D3DXVECTOR2(100,100), 
      };
      pLine->Begin();
      pLine->Draw(points, 2, D3DCOLOR_ARGB(255,255,255,255));
      pLine->End();
   } while(0);
   */
   timer.reset();
}

bool RootViewPlay::onInput(const GameTime* gt, const ::bootes::lib::framework::InputEvent* ev)
{
//   if (_ceguiView.onInput(ev)) { return true; }
   _pGameView->onInput(gt, ev);
   return false;
}

bool RootViewPlay::onSensorInput(const GameTime* gt, const ::bootes::lib::framework::InputEvent* ev)
{
   switch (ev->_type) {
   case ::bootes::lib::framework::InputEvent::T_WNDMSG:
      return false;
   case ::bootes::lib::framework::InputEvent::T_WIIMOTE:
   case ::bootes::lib::framework::InputEvent::T_KINECT:
      _pGameView->onSensorInput(gt, ev);
      return false;
   }
   return false;
}


/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
