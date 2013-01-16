#ifndef BOOTESDANCES_VIEW_STAGE_WIIMOTE_HANDLER_IMPL_H
#define BOOTESDANCES_VIEW_STAGE_WIIMOTE_HANDLER_IMPL_H

#include "../../include.h"
#include <bootes/dances/IWiimoteHandler.h>
#include <bootes/dances/Scene.h>
#include <bootes/dances/EvMovie.h>
#include <bootes/dances/EvStage.h>
#include "../../move/MoveSequence.h"

class WiimoteHandlerImpl: public IWiimoteHandler
                        , public ::bootes::lib::framework::EventListener
{
public:
   WiimoteHandlerImpl();
   virtual ~WiimoteHandlerImpl();

   bool init(MoveSequence*);
   void clear();

   virtual void teachClear(const IMoveModel*);
   virtual void teachBegin(const IMoveModel*);
   virtual void teachCommit(bool succeed);
   virtual void teachRollback();

   virtual void handleWiimote(const Scene*, const ::bootes::lib::framework::WiimoteEvent*);

   virtual void onSubscribe(::bootes::lib::framework::EventManager*);
   virtual void onUnsubscribe(::bootes::lib::framework::EventManager*);
   virtual void onEvent(const ::bootes::lib::framework::Event* ev);
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
   MoveModel* _pTestMove;
   TEST_MODE _test_mode;
   //MODE _ready_mode;
   MoveModel* _pTeachMove;
   bool _play;
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
