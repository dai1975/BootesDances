#include "RootViewEdit.h"
#include <bootes/lib/util/Timer.h>

#include <bootes/dances/EvStage.h>
#include <bootes/dances/EvMovie.h>

RootViewEdit::RootViewEdit(const TCHAR* dir)
{
   _pProxy = new StageManagerProxy();
   _pProxy->init(dir, true, D3DPOOL_SYSTEMMEM);

   _pGameView   = new GameView(_pProxy, D3DPOOL_MANAGED);
   _pEditorView = new ::bootes::dances::editor::EditorView();

   //   g_pFnd->getEventManager()->subscribe< EvLoadStageResult >(this);

   _pProxy->start();
}

RootViewEdit::~RootViewEdit()
{
   delete _pEditorView;
   delete _pGameView;
   delete _pProxy;
}

void RootViewEdit::onSubscribe(::bootes::lib::framework::EventManager*)
{
}
void RootViewEdit::onUnsubscribe(::bootes::lib::framework::EventManager*)
{
}
void RootViewEdit::onEvent(const ::bootes::lib::framework::GameTime* gt, const ::bootes::lib::framework::Event* p)
{
}

void RootViewEdit::onLostDevice()
{
   _pGameView->onLostDevice();
   _pEditorView->onLostDevice();
}

void RootViewEdit::onResetDevice()
{
   _pGameView->onResetDevice();
   _pEditorView->onResetDevice();
}

void RootViewEdit::onUpdate(const ::bootes::lib::framework::GameTime* gt)
{
   _pGameView->onUpdate(gt);
   _pEditorView->onUpdate(gt);
}

void RootViewEdit::onRender(const ::bootes::lib::framework::GameTime* gt)
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
   _pEditorView->onRender(gt);
   timer.get(NULL, &dt[ti++]);

   timer.reset();
}

bool RootViewEdit::onSensorInput(const ::bootes::lib::framework::GameTime* gt, const ::bootes::lib::framework::InputEvent* ev)
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

bool RootViewEdit::onInput(const ::bootes::lib::framework::GameTime* gt, const ::bootes::lib::framework::InputEvent* ev)
{
   _pEditorView->onInput(gt, ev);
   _pGameView->onInput(gt, ev);
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
