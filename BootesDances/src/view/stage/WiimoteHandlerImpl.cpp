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

   if (_editable) {
      _teachLogger.open(name);
   }

   return true;
}

void WiimoteHandlerImpl::onSubscribe(::bootes::lib::framework::EventManager*) { }
void WiimoteHandlerImpl::onUnsubscribe(::bootes::lib::framework::EventManager*) { }
void WiimoteHandlerImpl::onEvent(const ::bootes::lib::framework::Event* ev)
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

void WiimoteHandlerImpl::handleWiimote(const Scene* scene, const ::bootes::lib::framework::WiimoteEvent* ev)
{
   if (!_play) { return; }

   __int64 t = scene->clock().clock;

   if (_pTeachMove && _pTeachMove->getMotion()) {
      __int64 t0,t1;
      _pTeachMove->getTime(&t0, &t1);
      if (t0 <= t && t < t1) {
         __int64 rt = t - t0;
         _pTeachMove->getMotion()->teach(rt, ev);
         _teachSequence.add(rt,*ev);
      }
   }
   handleWiimoteTest(t, ev);
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
               _pTestMove->getMotion()->testEnd(true);
            }
         }
      }
      _pTestMove = NULL;
   }
   if (pMove == NULL) { return; }

   IMotion* pMotion = pMove->getMotion();
   __int64 t0,t1,rt;
   pMove->getTime(&t0, &t1);
   rt = t - t0;

   if (_pTestMove != pMove) {
      if (rt < 0) {
         _test_mode = M_READY;
      } else {
         if (pMotion) {
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
               pMotion->testBegin();
            }
            _test_mode = M_TEST;
            break;
         case M_TEST:
            if (pMotion) {
               pMotion->test(rt, ev);
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
