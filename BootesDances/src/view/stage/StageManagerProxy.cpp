#include "StageManagerProxy.h"
#include "StageRealizer.h"
#include "../../move/Move.h"
#include "../../move/motion/MotionRealizer.h"
#include <bootes/lib/util/TChar.h>
#include <sstream>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
//#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <share.h>
#include <fcntl.h>

StageManagerProxy::StageManagerProxy()
{
   _enabled = false;
   _pStage = NULL;
   _pSeq = NULL;

   _thread_handle = INVALID_HANDLE_VALUE;
   _mutex = NULL;
   _run = false;
   _command = NULL;

   g_pFnd->getEventManager()->subscribe< EvSearchStage >(this);
   g_pFnd->getEventManager()->subscribe< EvSaveStage >(this);
   g_pFnd->getEventManager()->subscribe< EvLoadStage >(this);
   g_pFnd->getEventManager()->subscribe< EvNewStage >(this);
   g_pFnd->getEventManager()->subscribe< EvPlayMovie >(this);
   g_pFnd->getEventManager()->subscribe< EvPauseMovie >(this);
   g_pFnd->getEventManager()->subscribe< EvSeekMovie >(this);
}

StageManagerProxy::~StageManagerProxy()
{
   stop();
   if (_pWiimoteHandler)  { delete _pWiimoteHandler; }
   if (_pMoveEditor)     { delete _pMoveEditor; }
   if (_pMovePresenter)  { delete _pMovePresenter; }
   if (_pStage) { delete _pStage; }
   if (_pSeq) { delete _pSeq; }
   delete _pMoviePlayer;
}

bool StageManagerProxy::init(const TCHAR* dir, bool editable, D3DPOOL pool)
{
   _dir = dir;
   _pStage = NULL;
   _pSeq = NULL;
   _editable = editable;
   _enabled = false;

   _pMoviePlayer    = new MoviePlayer(pool);
   _pMovePresenter  = new MovePresenterImpl(editable);
   _pMoveEditor     = new MoveEditorImpl();
   _pWiimoteHandler = new WiimoteHandlerImpl(editable, dir);

   {
      const MotionRealizer::Registry& reg = MotionRealizer::GetRegistry();
      for (MotionRealizer::Registry::const_iterator i = reg.begin(); i != reg.end(); ++i) {
         const MotionRealizer* f = i->second;
         _mgl.push_back( StageRealizer::MotionGuidePair(f->getMotionNameT(), f->getGuideNameT()) );
      }
   }

   return true;
}

IMove* StageManagerProxy::createMove(IGuide* pGuide) const
{
   if (! _enabled) { return NULL; }
   if (_pStage == NULL || _pSeq == NULL) { return NULL; }
   Move* pMove = new Move();
   pMove->setGuide(pGuide);
   pMove->setMotion(_pSeq->getMotionRealizer()->createMotion(0));
   return pMove;
}

DWORD WINAPI StageManagerProxy::ThreadProc(LPVOID param)
{
   StageManagerProxy* p = static_cast< StageManagerProxy* >(param);
   return p->run();
}

bool StageManagerProxy::start()
{
   if (_thread_handle != INVALID_HANDLE_VALUE) {
      return true;
   }
   _mutex = CreateMutex(NULL, FALSE, NULL);
   if (_mutex == NULL) {
      return false;
   }

   _thread_handle = CreateThread(NULL, 0, &StageManagerProxy::ThreadProc, this, 0, &_thread_id);
   return (_thread_handle != INVALID_HANDLE_VALUE);
}

void StageManagerProxy::stopLater()
{
   _run = false;
}

void StageManagerProxy::stop()
{
   if (_thread_handle == INVALID_HANDLE_VALUE) { return; }
   stopLater();
   WaitForSingleObject(_thread_handle, INFINITE);
   _thread_handle = INVALID_HANDLE_VALUE;
   CloseHandle(_mutex);
}

DWORD StageManagerProxy::run()
{
   if (FAILED( CoInitializeEx(NULL, COINIT_APARTMENTTHREADED) )) {
      return 0;
   }

   _run = true;
   while (_run) {
      Sleep(1);
      if (_command == NULL) {
         continue;
      }

      WaitForSingleObject(_mutex, INFINITE);
      if (_command) {
         ::bootes::lib::framework::Event* cmd = _command;

         intptr_t cid = cmd->getEventId();
         if (cid == EvSearchStage::GetEventId()) {
            doSearch();
         } else if (cid == EvSaveStage::GetEventId()) {
            EvSaveStage* e = static_cast< EvSaveStage* >(cmd);
            doSave(e->_basename.c_str(), e->_new);
         } else if (cid == EvLoadStage::GetEventId()) {
            EvLoadStage* e = static_cast< EvLoadStage* >(cmd);
            doLoad(e->_basename.c_str());
         } else if (cid == EvNewStage::GetEventId()) {
            EvNewStage* e = static_cast< EvNewStage* >(cmd);
            doNew(e->_moviepath.c_str());
         }
         delete cmd;
         _command = NULL;
      }
      ReleaseMutex(_mutex);
      Sleep(1);
   }
   CoUninitialize();
   return 0;
}

void StageManagerProxy::onSubscribe(::bootes::lib::framework::EventManager*) { }
void StageManagerProxy::onUnsubscribe(::bootes::lib::framework::EventManager*) { }

void StageManagerProxy::onEvent(const ::bootes::lib::framework::Event* ev)
{
   bool locked = false;
   if (_command != NULL) { goto fail; }

   locked = (WaitForSingleObject(_mutex, 0) == WAIT_OBJECT_0);
   if (!locked) { goto fail; }
   if (_command != NULL) { goto fail; }

   if (! onEvent0(ev)) { goto fail; }
   ReleaseMutex(_mutex);
   return;

 fail:
   if (locked) {
      ReleaseMutex(_mutex);
   }
   int eid = ev->getEventId();
   if (false) {
   } else if (eid == EvSearchStage::GetEventId()) {
      EvSearchStageResult r;
      r._result = false;
      g_pFnd->queue(&r);
   } else if (eid == EvSaveStage::GetEventId()) {
      EvSaveStageResult r;
      r._result = false;
      g_pFnd->queue(&r);
   } else if (eid == EvLoadStage::GetEventId()) {
      EvLoadStageResult r;
      r._result = false;
      g_pFnd->queue(&r);
   } else if (eid == EvNewStage::GetEventId()) {
      EvNewStageResult r;
      r._result = false;
      g_pFnd->queue(&r);
   }
   return;
}

bool StageManagerProxy::onEvent0(const ::bootes::lib::framework::Event* ev)
{
   if (tryDispatch(ev, &StageManagerProxy::doPlay)) { return true; }
   else if (tryDispatch(ev, &StageManagerProxy::doPause)) { return true; }
   else if (tryDispatch(ev, &StageManagerProxy::doSeek)) { return true; }

   if (ev->getEventId() == EvSaveStage::GetEventId()) {
      const EvSaveStage* e = static_cast< const EvSaveStage* >(ev);
   } else if (ev->getEventId() == EvLoadStage::GetEventId()) {
      const EvLoadStage* e = static_cast< const EvLoadStage* >(ev);
   } else if (ev->getEventId() == EvNewStage::GetEventId()) {
      const EvNewStage* e = static_cast< const EvNewStage* >(ev);
   }
   _command = ev->clone();
   return true;
}

void StageManagerProxy::doSearch()
{
   struct Callback {
      static void f(bool result, Stage* pStage, void* data) {
         EvSearchStageResult r;
         r._result = result;
         r._pStage.reset(pStage);
         int* pIndex = static_cast< int* >(data);
         r._index = *pIndex;
         ++*pIndex;
         g_pFnd->queue(&r);
      }
   };
   int index = 0;
   StageRealizer::Search(&Callback::f, &index, _dir.c_str(), _mgl);
}

void StageManagerProxy::doSave(const TCHAR* basename, bool neu)
{
   EvSaveStageResult res;
   res._result    = false;
   res._basename  = basename;

   if (_pStage == NULL || _pSeq == NULL) {
      g_pFnd->queue(&res);
      return;
   }

   if (StageRealizer::Save(&res._basename, _dir.c_str(), basename, neu, *_pStage, *_pSeq)) {
      res._result = true;
      _pStage->tc_basename = res._basename;
   }
   g_pFnd->queue(&res);
   return;
}

void StageManagerProxy::doNew(const TCHAR* moviepath)
{
   Stage* pStage      = NULL;
   MoveSequence* pSeq = NULL;

   EvNewStageResult r;
   r._result = false;
   r._moviepath = moviepath;
   if (moviepath == _T('\0')) { goto fail; }
   if (_mgl.empty()) { goto fail; }

   if (! StageRealizer::New(&pStage, &pSeq, *_mgl.begin())) { goto fail; }

   // moviepath を解析して、ファイル名の . 以前を basename として得る
   {
      const TCHAR *pc0 = moviepath;
      const TCHAR *pc1 = NULL;
      const TCHAR* pc;
      for (pc=moviepath; *pc != _T('\0'); ++pc) {
         if (*pc == _T('\\')) { pc0 = pc; }
         else if (*pc == _T('.')) { pc1 = pc; }
      }
      ++pc0;
      if (pc1 == NULL || pc1 < pc0) {
         pc1 = pc;
      }

      pStage->version = 1;
      pStage->tc_moviepath = moviepath;
      pStage->tc_basename.assign(pc0, pc1-pc0);
      {
         char* tmp;
         tmp = ::bootes::lib::util::TChar::T2C(pStage->tc_basename.c_str());
         if (tmp != NULL) {
            pStage->name = tmp;
            delete[] tmp;
         }

         tmp = ::bootes::lib::util::TChar::T2C(moviepath);
         if (tmp != NULL) {
            pStage->moviepath = tmp;
            delete[] tmp;
         }
      }
   }

   //update _pStage, _pSeq, _enable
   if (! doLoadMovie(moviepath, pStage, pSeq)) { goto fail; }

   r._result    = true;
   r._basename  = _pStage->tc_basename;
   r._videoInfo = _pMoviePlayer->getVideoInfo();
   g_pFnd->queue(&r);
   return;

fail:
   if (pStage) { delete pStage; }
   if (pSeq) { delete pSeq; }
   r._result = false;
   g_pFnd->queue(&r);
   return;
}

void StageManagerProxy::doLoad(const TCHAR* name)
{
   EvLoadStageResult r;
   r._result = false;

   Stage* pStage = NULL;
   MoveSequence* pSeq = NULL;

   if (! StageRealizer::Load(&pStage, &pSeq, _dir.c_str(), name, _mgl)) { goto fail; }
   if (! doLoadMovie(pStage->tc_moviepath.c_str(), pStage, pSeq)) { goto fail; } //update _pStage, _pSeq, _enable

   r._result    = true;
   r._basename  = _pStage->tc_basename;
   r._videoInfo = _pMoviePlayer->getVideoInfo();
   g_pFnd->queue(&r);
   return;

 fail:
   r._result = false;
   g_pFnd->queue(&r);
   return;
}

bool StageManagerProxy::doLoadMovie(const TCHAR* path, Stage* pStage, MoveSequence* pSeq)
{
   _enabled = false;
   if (! _pMoviePlayer->load(path)) { goto fail; }

   if (_pMovePresenter)  { if (!_pMovePresenter->initStage(pSeq, _pMoveEditor)) { goto fail; } }
   if (_pMoveEditor)     { if (!_pMoveEditor->initStage(pSeq)) { goto fail; } }
   if (_pWiimoteHandler) { if (!_pWiimoteHandler->initStage(pSeq, pStage->tc_basename.c_str())) { goto fail; } }

   if (_pStage) { delete _pStage; }
   if (_pSeq) { delete _pSeq; }
   _pStage   = pStage;
   _pSeq     = pSeq;
   _enabled  = true;

   return true;

 fail:
   _enabled = false;
   if (pStage) { delete pStage; }
   if (pSeq) { delete pSeq; }
   _pStage = NULL;
   _pSeq = NULL;
   if (_pMovePresenter)  { _pMovePresenter->initStage(NULL, _pMoveEditor); }
   if (_pMoveEditor)     { _pMoveEditor->initStage(NULL); }
   if (_pWiimoteHandler) { _pWiimoteHandler->initStage(NULL, NULL); }
   return false;
}

void StageManagerProxy::doPlay(const EvPlayMovie* ev)
{
   if (_pStage == NULL) { return; }
   if (! _pMoviePlayer->isValid()) { return; }
   if (! _pMoviePlayer->play()) { return; }

   EvMoviePlay n;
   n._time = _pMoviePlayer->getStageClock().clock;
   g_pFnd->getEventManager()->queue(&n);
}

void StageManagerProxy::doPause(const EvPauseMovie* ev)
{
   if (_pStage == NULL) { return; }
   if (! _pMoviePlayer->isValid()) { return; }
   if (! _pMoviePlayer->pause()) { return; }

   EvMoviePause n;
   n._time = _pMoviePlayer->getStageClock().clock;
   g_pFnd->getEventManager()->queue(&n);
}

void StageManagerProxy::doSeek(const EvSeekMovie* ev)
{
   if (_pStage == NULL) { return; }
   if (! _pMoviePlayer->isValid()) { return; }

   __int64 t0 = _pMoviePlayer->getStageClock().clock;
   switch (ev->_origin)
   {
   case EvSeekMovie::SET:
      _pMoviePlayer->seek_set(ev->_offset); break;
   case EvSeekMovie::CUR:
      _pMoviePlayer->seek_cur(ev->_offset); break;
   case EvSeekMovie::END:
      _pMoviePlayer->seek_end(ev->_offset); break;
   }
   __int64 t1 = _pMoviePlayer->getStageClock().clock;
   if (t0 != t1) {
      EvMovieSeek n;
      n._time = t1;
      g_pFnd->getEventManager()->queue(&n);
   }

   if (ev->_play) {
      if (_pMoviePlayer->play()) {
         EvMoviePlay n;
         n._time = _pMoviePlayer->getStageClock().clock;
         g_pFnd->getEventManager()->queue(&n);
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
