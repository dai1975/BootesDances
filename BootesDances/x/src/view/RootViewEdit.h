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
   virtual void onUpdate(double currentTime, int elapsedTime);
   virtual void onRender(double currentTime, int elapsedTime);
   virtual bool onInput(const ::bootes::lib::framework::InputEvent*);
   virtual void onLostDevice();
   virtual void onResetDevice();

public:
   inline virtual bool hasStage() const { return _pProxy->hasStage(); }
   inline virtual const ::pb::Stage* getStage() const { return _pProxy->getStage(); }
   inline virtual ISceneSequencer* getSceneSequencer() { return _pGameView; }
   inline virtual IMovePresenter*  getMovePresenter()  { return _pProxy->getMovePresenter(); }
   inline virtual IMoveEditor*     getMoveEditor()     { return _pProxy->getMoveEditor(); }
   inline virtual IWiimoteHandler* getWiimoteHandler() { return _pProxy->getWiimoteHandler(); }

public:
   virtual void onSubscribe(::bootes::lib::framework::EventManager*);
   virtual void onUnsubscribe(::bootes::lib::framework::EventManager*);
   virtual void onEvent(const ::bootes::lib::framework::Event* ev);

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