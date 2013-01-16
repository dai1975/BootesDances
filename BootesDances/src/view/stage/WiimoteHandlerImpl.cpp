#include "WiimoteHandlerImpl.h"

WiimoteHandlerImpl::WiimoteHandlerImpl()
{
   _pMoves = NULL;
   _pTestMove = NULL;
   _pTeachMove = NULL;
   _play = false;

   g_pFnd->getEventManager()->subscribe< EvLoadStageResult >(this);
   g_pFnd->getEventManager()->subscribe< EvMoviePlay >(this);
   g_pFnd->getEventManager()->subscribe< EvMoviePause >(this);
   g_pFnd->getEventManager()->subscribe< EvMovieSeek >(this);
}

WiimoteHandlerImpl::~WiimoteHandlerImpl()
{
}

bool WiimoteHandlerImpl::init(MoveSequence* moves)
{
   _pMoves = moves;
   _pTestMove = NULL;
   _pTeachMove = NULL;
   _play = false;
   return true;
}

void WiimoteHandlerImpl::clear()
{
   _pTestMove = NULL;
   _pTeachMove = NULL;
}

void WiimoteHandlerImpl::onSubscribe(::bootes::lib::framework::EventManager*) { }
void WiimoteHandlerImpl::onUnsubscribe(::bootes::lib::framework::EventManager*) { }
void WiimoteHandlerImpl::onEvent(const ::bootes::lib::framework::Event* ev)
{
   if (tryDispatch(ev, &WiimoteHandlerImpl::onLoad)) { return; }
   else if (tryDispatch(ev, &WiimoteHandlerImpl::onPlay)) { return; }
   else if (tryDispatch(ev, &WiimoteHandlerImpl::onPause)) { return; }
   else if (tryDispatch(ev, &WiimoteHandlerImpl::onSeek)) { return; }
}

void WiimoteHandlerImpl::onLoad(const EvLoadStageResult* ev)
{
   clear();
   _pTestMove = NULL;
}

void WiimoteHandlerImpl::onPlay(const EvMoviePlay* ev)
{
   ::bootes::lib::framework::WiimoteEvent dummy;
   for (MoveSequence::iterator i = _pMoves->begin(); i != _pMoves->end(); ++i) {
      static_cast< MoveModel* >(*i)->getMotion(Motion::WIIMOTE)->testClear();
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

void WiimoteHandlerImpl::teachClear(const IMoveModel* pMove)
{
   MoveSequence::iterator i = _pMoves->search(pMove);
   if (i == _pMoves->end()) { return; }

   MoveModel* p = static_cast< MoveModel* >(*i);
   p->getMotion(Motion::WIIMOTE)->teachClear();
}
void WiimoteHandlerImpl::teachBegin(const IMoveModel* pMove)
{
   MoveSequence::iterator i = _pMoves->search(pMove);
   if (i == _pMoves->end()) { return; }
   _pTeachMove = static_cast< MoveModel* >(*i);
   _pTeachMove->getMotion(Motion::WIIMOTE)->teachBegin();
}
void WiimoteHandlerImpl::teachCommit(bool succeed)
{
   if (_pTeachMove == NULL) { return; }

   _pTeachMove->getMotion(Motion::WIIMOTE)->teachCommit(succeed);
   _pTeachMove = NULL;
}
void WiimoteHandlerImpl::teachRollback()
{
   if (_pTeachMove == NULL) { return; }

   ::bootes::lib::framework::WiimoteEvent ev;
   _pTeachMove->getMotion(Motion::WIIMOTE)->teachRollback();
   _pTeachMove = NULL;
}

void WiimoteHandlerImpl::handleWiimote(const Scene* scene, const ::bootes::lib::framework::WiimoteEvent* ev)
{
   if (!_play) { return; }

   __int64 t = scene->clock().clock;

   if (_pTeachMove) {
      __int64 rt = t - _pTeachMove->getBeginTime();
      _pTeachMove->getMotion(Motion::WIIMOTE)->teach(ev, rt);
   }
   handleWiimoteTest(t, ev);
}

void WiimoteHandlerImpl::testClear()
{
   if (_pTestMove) {
      _pTestMove->getMotion(Motion::WIIMOTE)->testEnd(false);
   }
   _pTestMove = NULL;
   _test_mode = M_READY;
}

void WiimoteHandlerImpl::handleWiimoteTest(__int64 t, const ::bootes::lib::framework::WiimoteEvent* ev)
{
   MoveModel* pm = NULL;
   {
      MoveSequence::iterator i = _pMoves->searchGE(t, false);
      if (i != _pMoves->end()) {
         pm = static_cast< MoveModel* >(*i);
      }
   }

   if (_pTestMove != pm) {
      if (_pTestMove != NULL) {
         switch (_test_mode) {
         case M_TEST:
            _pTestMove->getMotion(Motion::WIIMOTE)->testEnd(true);
         }
      }
      _pTestMove = NULL;
   }
   if (pm == NULL) { return; }

   Motion* pMotion = pm->getMotion(Motion::WIIMOTE);
   __int64 t0,t1,rt;
   pm->getTime(&t0, &t1);
   rt = t - t0;

   if (_pTestMove != pm) {
      if (rt < 0) {
         _test_mode = M_READY;
      } else {
         pMotion->testBegin();
         pMotion->test(ev, rt);
         _test_mode = M_TEST;
      }
      _pTestMove = pm;
      
   } else {
      if (rt < 0) {
         _test_mode = M_READY;
      } else {
         switch (_test_mode) {
         case M_READY:
            pMotion->testBegin();
            _test_mode = M_TEST;
            break;
         case M_TEST:
            pMotion->test(ev, rt);
            break;
         case M_BREAK:
            break;
         default:
            pMotion->testEnd(false);
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
