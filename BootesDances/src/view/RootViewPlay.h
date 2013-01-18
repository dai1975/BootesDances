#ifndef BOOTESDANCES_VIEW_ROOTVIEWPLAY_H
#define BOOTESDANCES_VIEW_ROOTVIEWPLAY_H

#include "../include.h"
#include "RootView.h"
#include "stage/StageManagerProxy.h"
#include "GameView.h"

class RootViewPlay: public RootView
                  , public ::bootes::lib::framework::EventListener
                  , public IStageManager
{
public:
   explicit RootViewPlay(const TCHAR* dir);
   virtual ~RootViewPlay();
   inline virtual IStageManager* getStageManager() { return this; }

public:
   virtual void onUpdate(double currentTime, int elapsedTime);
   virtual void onRender(double currentTime, int elapsedTime);
   virtual bool onInput(const ::bootes::lib::framework::InputEvent*);
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
   virtual void onEvent(const ::bootes::lib::framework::Event* ev);

private:
   StageManagerProxy* _pProxy;
   GameView* _pGameView;
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
