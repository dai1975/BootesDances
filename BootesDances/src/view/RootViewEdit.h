#ifndef BOOTESDANCES_VIEW_ROOTVIEWEDIT_H
#define BOOTESDANCES_VIEW_ROOTVIEWEDIT_H

#include "../include.h"
#include "RootView.h"
#include "stage/StageManagerProxy.h"
#include "GameView.h"
#include "editor/EditorView.h"

class RootViewEdit: public RootView
                  , public ::bootes::lib::framework::EventListener
                  , public IStageManager
{
public:
   explicit RootViewEdit(const TCHAR* dir);
   virtual ~RootViewEdit();
   inline virtual IStageManager* getStageManager() { return this; }

public:
   virtual void onUpdate(const GameTime* gt);
   virtual void onRender(const GameTime* gt);
   virtual bool onInput(const GameTime* gt, const ::bootes::lib::framework::InputEvent*);
   virtual bool onSensorInput(const GameTime* gt, const ::bootes::lib::framework::InputEvent*);
   virtual void onLostDevice();
   virtual void onResetDevice();

public:
   inline virtual bool isEnabled() const { return _pProxy->isEnabled(); }
   inline virtual IMove* createMove(IGuide* p) const { return _pProxy->createMove(p); }
   inline virtual ISceneSequencer* getSceneSequencer() { return _pGameView; }
   inline virtual IMovePresenter*  getMovePresenter()  { return _pProxy->getMovePresenter(); }
   inline virtual IMoveEditor*     getMoveEditor()     { return _pProxy->getMoveEditor(); }
   inline virtual IWiimoteHandler* getWiimoteHandler() { return _pProxy->getWiimoteHandler(); }

public:
   virtual void onSubscribe(::bootes::lib::framework::EventManager*);
   virtual void onUnsubscribe(::bootes::lib::framework::EventManager*);
   virtual void onEvent(const ::bootes::lib::framework::GameTime*, const ::bootes::lib::framework::Event* ev);

private:
   StageManagerProxy* _pProxy;
   GameView*    _pGameView;
   ::bootes::dances::editor::EditorView*   _pEditorView;
};

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
