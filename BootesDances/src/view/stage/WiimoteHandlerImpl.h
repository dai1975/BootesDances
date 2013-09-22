#ifndef BOOTESDANCES_VIEW_STAGE_WIIMOTE_HANDLER_IMPL_H
#define BOOTESDANCES_VIEW_STAGE_WIIMOTE_HANDLER_IMPL_H

#include "../../include.h"
#include <bootes/dances/IWiimoteHandler.h>
#include <bootes/dances/Scene.h>
#include <bootes/dances/EvMovie.h>
#include <bootes/dances/EvStage.h>
#include "../../move/MoveSequence.h"
#include "../../move/motion/TeachLog.h"

class WiimoteHandlerImpl: public IWiimoteHandler
                        , public ::bootes::lib::framework::EventListener
{
public:
   WiimoteHandlerImpl(bool editable, const TCHAR* dir);
   virtual ~WiimoteHandlerImpl();

   bool initStage(MoveSequence*, const TCHAR* name);
   //void clear();

   virtual void teachClear(const IMove*);
   virtual void teachBegin(const IMove*);
   virtual void teachCommit(bool succeed);
   virtual void teachRollback();

   virtual void handleWiimote(const GameTime*, const Scene*, const WiimoteEvent*);

   virtual void onSubscribe(::bootes::lib::framework::EventManager*);
   virtual void onUnsubscribe(::bootes::lib::framework::EventManager*);
   virtual void onEvent(const GameTime*, const ::bootes::lib::framework::Event* ev);
   void onLoad(const EvLoadStageResult* ev);
   void onPlay(const EvMoviePlay* ev);
   void onPause(const EvMoviePause* ev);
   void onSeek(const EvMovieSeek* ev);

private:
   enum TEST_MODE {
      M_READY, M_TEST, M_BREAK,
   };
   //void handle(MODE mode, const Scene* scene, const ::bootes::lib::framework::WiimoteEvent*);
   virtual void handleWiimoteTest(__int64 t, const ::bootes::lib::framework::WiimoteEvent*);
   void testClear();

   MoveSequence *_pMoves;
   IMove* _pTestMove;
   TEST_MODE _test_mode;
   //MODE _ready_mode;
   IMove* _pTeachMove;
   _int64 _teach_evtime_to_scenetime;
   _int64 _test_evtime_to_scenetime;

   bool _play;
   bool _editable;
   TeachLogger _teachLogger;
   TeachSequence _teachSequence;
};

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
