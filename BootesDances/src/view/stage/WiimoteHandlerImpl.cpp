#include "WiimoteHandlerImpl.h"

WiimoteHandlerImpl::WiimoteHandlerImpl(bool editable, const TCHAR* dir)
{
   _pMoves = NULL;
   _pTestMove = NULL;
   _pTeachMove = NULL;
   _play = false;
   _editable = editable;

   if (_editable) {
      _teachLogger.start(dir);
   }

   g_pFnd->getEventManager()->subscribe< EvLoadStageResult >(this);
   g_pFnd->getEventManager()->subscribe< EvMoviePlay >(this);
   g_pFnd->getEventManager()->subscribe< EvMoviePause >(this);
   g_pFnd->getEventManager()->subscribe< EvMovieSeek >(this);
}

WiimoteHandlerImpl::~WiimoteHandlerImpl()
{
   if (_editable && _teachLogger.isRun()) {
      _teachLogger.join();
   }
}

bool WiimoteHandlerImpl::initStage(MoveSequence* moves, const TCHAR* name)
{
   _pMoves = moves;
   _pTestMove = NULL;
   _pTeachMove = NULL;
   _play = false;
   _teachLogger.join();

   if (_editable && name) {
      _teachLogger.open(name);
   }

   return true;
}

void WiimoteHandlerImpl::onSubscribe(::bootes::lib::framework::EventManager*) { }
void WiimoteHandlerImpl::onUnsubscribe(::bootes::lib::framework::EventManager*) { }
void WiimoteHandlerImpl::onEvent(const ::bootes::lib::framework::GameTime* gt, const ::bootes::lib::framework::Event* ev)
{
   if (false) { ; }
   else if (tryDispatch(ev, &WiimoteHandlerImpl::onPlay)) { return; }
   else if (tryDispatch(ev, &WiimoteHandlerImpl::onPause)) { return; }
   else if (tryDispatch(ev, &WiimoteHandlerImpl::onSeek)) { return; }
}

void WiimoteHandlerImpl::onPlay(const EvMoviePlay* ev)
{
   ::bootes::lib::framework::WiimoteEvent dummy;
   for (MoveSequence::iterator i = _pMoves->begin(); i != _pMoves->end(); ++i) {
      IMotion* p = (*i)->getMotion();
      if (p) {
         p->testClear();
      }
   }
   testClear();
   _play = true;
}

void WiimoteHandlerImpl::onPause(const EvMoviePause* ev)
{
   _play = false;
   testClear();
}

void WiimoteHandlerImpl::onSeek(const EvMovieSeek* ev)
{
   _play = false;
   testClear();
}

void WiimoteHandlerImpl::teachClear(const IMove* pMove_)
{
   MoveSequence::iterator i = _pMoves->search(pMove_);
   if (i == _pMoves->end()) { return; }

   IMove* pMove = *i;
   IMotion* pMotion = pMove->getMotion();
   if (pMotion) {
      pMotion->teachClear();
   }
   _teachLogger.add_clear(pMove->getUuid());
}
void WiimoteHandlerImpl::teachBegin(const IMove* pMove)
{
   MoveSequence::iterator i = _pMoves->search(pMove);
   if (i == _pMoves->end()) { return; }
   IMotion* p = (*i)->getMotion();
   if (p) {
      p->teachBegin();
      _teachSequence.clear();
   }
   _pTeachMove = *i;
   _teach_evtime_to_scenetime = 0;
}
void WiimoteHandlerImpl::teachCommit(bool succeed)
{
   if (_pTeachMove == NULL) { return; }

   IMotion* p = _pTeachMove->getMotion();
   if (p) {
      p->teachCommit(succeed);
   }
   _teachSequence.commit(succeed);
   _teachLogger.add_sequence(_pTeachMove->getUuid(), _teachSequence);
   _pTeachMove = NULL;
}
void WiimoteHandlerImpl::teachRollback()
{
   if (_pTeachMove == NULL) { return; }

   ::bootes::lib::framework::WiimoteEvent ev;
   IMotion* p = _pTeachMove->getMotion();
   if (p) {
      p->teachRollback();
   }
   _pTeachMove = NULL;
}

void WiimoteHandlerImpl::handleWiimote(const GameTime* gt, const Scene* scene, const WiimoteEvent* wev)
{
   if (!_play) { return; }

   __int64 t = scene->clock().clock;

   if (_pTeachMove && _pTeachMove->getMotion()) {
      __int64 t0,t1;
      _pTeachMove->getTime(&t0, &t1);
      if (t0 <= t && t < t1) {
//         static std::list< __int64 > lst0;
//         static std::list< __int64 > lst1;
//         static std::list< __int64 > lst2;

         __int64 evtime = wev->_event_timestamp * 10000; // [ms] to [100ns]
         if (_teach_evtime_to_scenetime == 0) { //scene 時刻は飛び飛びなので evtime を使う。一時停止に注意。
            __int64 rt = t - t0;
            _teach_evtime_to_scenetime = evtime - rt;
         }
         __int64 evrt = evtime - _teach_evtime_to_scenetime;
//         lst0.push_back(t);
//         lst1.push_back(evtime);
//         lst2.push_back(evrt);

         _pTeachMove->getMotion()->teach(evrt, wev);
         _teachSequence.add(evrt, *wev);
      }
   }
   handleWiimoteTest(t, wev);
}

void WiimoteHandlerImpl::testClear()
{
   if (_pTestMove && _pTestMove->getMotion()) {
      _pTestMove->getMotion()->testEnd(false);
   }
   _pTestMove = NULL;
   _test_mode = M_READY;
}

void WiimoteHandlerImpl::handleWiimoteTest(__int64 t, const ::bootes::lib::framework::WiimoteEvent* ev)
{
   IMove* pMove = NULL;
   {
      MoveSequence::iterator i = _pMoves->searchGE(t, false);
      if (i != _pMoves->end()) {
         pMove = *i;
      }
   }

   if (_pTestMove != pMove) {
      if (_pTestMove != NULL) {
         switch (_test_mode) {
         case M_TEST:
            if (_pTestMove->getMotion()) {
               __int64 t0,t1,rt;
               _pTestMove->getTime(&t0, &t1);
               rt = t - t0;
               _pTestMove->getMotion()->test(rt, ev);
               _pTestMove->getMotion()->testEnd(true);
            }
         }
      }
      _pTestMove = NULL;
   }
   if (pMove == NULL) { return; }

   IMotion* pMotion = pMove->getMotion();
   __int64 evtime = ev->_event_timestamp * 10000; // [ms] to [100ns]
   __int64 t0,t1,rt;
   pMove->getTime(&t0, &t1);
   rt = t - t0;

   if (_pTestMove != pMove) {
      if (rt < 0) {
         _test_mode = M_READY;
      } else {
         if (pMotion) {
            _test_evtime_to_scenetime = evtime - rt;
            pMotion->testBegin();
            pMotion->test(rt, ev);
         }
         _test_mode = M_TEST;
      }
      _pTestMove = pMove;
      
   } else {
      if (rt < 0) {
         _test_mode = M_READY;
      } else {
         switch (_test_mode) {
         case M_READY:
            if (pMotion) {
               _test_evtime_to_scenetime = evtime - rt;
               pMotion->testBegin();
            }
            _test_mode = M_TEST;
            break;
         case M_TEST:
            if (pMotion) {
               __int64 evrt = evtime - _test_evtime_to_scenetime;
               pMotion->test(evrt, ev);
            }
            break;
         case M_BREAK:
            break;
         default:
            if (pMotion) {
               pMotion->testEnd(false);
            }
            _test_mode = M_BREAK;
            break;
         }
      }
   }
}

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
