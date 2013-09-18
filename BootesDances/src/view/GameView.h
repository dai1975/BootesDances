#ifndef BOOTESDANCES_VIEW_GAMEVIEW_H
#define BOOTESDANCES_VIEW_GAMEVIEW_H

#include "../include.h"
#include "stage/StageManagerProxy.h"
#include <bootes/lib/d3dx9/Label.h>
#include <bootes/dances/EvStage.h>
#include "TextureView.h"

class GameView: public ISceneSequencer
              , public ::bootes::lib::framework::EventListener
{
public:
   GameView(StageManagerProxy*, D3DPOOL);
   ~GameView();

   virtual Scene getScene(bool requireTexture) const;
   virtual StageClock getStageClock() const;
   virtual VideoInfo  getVideoInfo() const;

public:
   bool onSensorInput(const ::bootes::lib::framework::GameTime* gt, const ::bootes::lib::framework::InputEvent*);
   bool onInput(const ::bootes::lib::framework::GameTime* gt, const ::bootes::lib::framework::InputEvent*);
   void onUpdate(const ::bootes::lib::framework::GameTime* gt);
   void onRender(const ::bootes::lib::framework::GameTime* gt);
   void onLostDevice();
   void onResetDevice();
   void onEvent(const ::bootes::lib::framework::GameTime* gt, const ::bootes::lib::framework::Event* ev);

private:
   void onRender0(IDirect3DDevice9* pDev, const ::bootes::lib::framework::GameTime* gt, const Scene* scene);
//   void clearBuffer();
//   HRESULT createBuffer(IDirect3DTexture9*);
   void drawFps(IDirect3DDevice9*, const ::bootes::lib::framework::GameTime* gt, const Scene*);
   
   enum STATE {
      S_0, S_RUN,
   };
   STATE _state;

   TextureView _texView;
   D3DPOOL _pool;
/*
   IDirect3DSurface9* _pRenderSurface;
   IDirect3DSurface9* _pDepthSurface;
   IDirect3DTexture9* _pExportTexture;
   IDirect3DSurface9* _pExportTmpSurface;
*/
   ::bootes::lib::d3dx9::Label _label;
   MoviePlayer*       _pMoviePlayer;
   MovePresenterImpl* _pMovePresenter;
   IWiimoteHandler*   _pWiimoteHandler;
   StageManagerProxy* _pStageManager;
   bool _enabled;
};

#endif

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
