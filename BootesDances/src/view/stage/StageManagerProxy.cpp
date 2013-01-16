#include "StageManagerProxy.h"
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
   _pStage = NULL;

   _thread_handle = INVALID_HANDLE_VALUE;
   _mutex = NULL;
   _run = false;
   _command = NULL;

   g_pFnd->getEventManager()->subscribe< EvSearchStage >(this);
   g_pFnd->getEventManager()->subscribe< EvSaveStage >(this);
   g_pFnd->getEventManager()->subscribe< EvLoadStage >(this);
   g_pFnd->getEventManager()->subscribe< EvPlayMovie >(this);
   g_pFnd->getEventManager()->subscribe< EvPauseMovie >(this);
   g_pFnd->getEventManager()->subscribe< EvSeekMovie >(this);
}

bool StageManagerProxy::init(const TCHAR* dir, bool editable, D3DPOOL pool)
{
   _dir = dir;

   _pMoviePlayer    = new MoviePlayer(pool);
   _pMovePresenter  = new MovePresenterImpl(editable);
   _pMoveEditor     = new MoveEditorImpl();
   _pWiimoteHandler = new WiimoteHandlerImpl();

   if (_pMovePresenter)  { if (! _pMovePresenter->init(&_moves, _pMoveEditor)) { return false; } }
   if (_pMoveEditor)     { if (! _pMoveEditor->init(&_moves)) { return false; } }
   if (_pWiimoteHandler) { if (! _pWiimoteHandler->init(&_moves)) { return false; } }
   return true;
}

StageManagerProxy::~StageManagerProxy()
{
   stop();
   if (_pWiimoteHandler)  { delete _pWiimoteHandler; }
   if (_pMoveEditor)     { delete _pMoveEditor; }
   if (_pMovePresenter)  { delete _pMovePresenter; }
   if (_pStage) { delete _pStage; }
   delete _pMoviePlayer;
}

void StageManagerProxy::clear()
{
   if (_pStage) {
      ::pb::Stage* p = _pStage;
      _pStage = NULL;
      delete p;
   }

   _moves.clear();
   _pMoviePlayer->clear();
   if (_pMovePresenter) { _pMovePresenter->clear(); }
   if (_pMoveEditor) { _pMoveEditor->clear(); }
   if (_pWiimoteHandler) { _pWiimoteHandler->clear(); }
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
            doSearch(_dir.c_str());
         } else if (cid == EvSaveStage::GetEventId()) {
            EvSaveStage* e = static_cast< EvSaveStage* >(cmd);
            doSave(e->_filepath.c_str());
         } else if (cid == EvLoadStage::GetEventId()) {
            EvLoadStage* e = static_cast< EvLoadStage* >(cmd);
            doLoad(e->_stage);
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
      ;
   } else if (ev->getEventId() == EvLoadStage::GetEventId()) {
      const EvLoadStage* e = static_cast< const EvLoadStage* >(ev);
      if (e->_stage.operator->() == NULL) { return false; }
   }
   _command = ev->clone();
   return true;
}

namespace {
   inline bool CheckSuffixStage(const TCHAR* str) {
      size_t len = _tcslen(str);
      return (4 <= len && _tcscmp(_T(".flx"), &str[len-4]) == 0);
   }
}

namespace {
static bool checkStage(::pb::Stage* stage)
{
   for (size_t i=0; i<stage->moviepath().size(); ++i) {
      switch (stage->moviepath().at(i)) {
      case '/':
         stage->mutable_moviepath()->at(i) = '\\'; break;
      case ':':
         return false;
      }
   }
   return true;
}
}

void StageManagerProxy::doSearch(const TCHAR* dir)
{
   typedef std::basic_string<TCHAR> tc_string;
   typedef std::list< tc_string >   tc_string_list;

   EvSearchStageResult r;
   WIN32_FIND_DATA find;
   HANDLE hFind;
   {
      tc_string query = dir;
      query += _T("\\*");
      hFind = FindFirstFile(query.c_str(), &find);
      if (hFind == INVALID_HANDLE_VALUE) {
         r._result = false;
         g_pFnd->queue(&r);
         return;
      }
   }

   for (BOOL found = TRUE; found; found=FindNextFile(hFind, &find)) {
      if ((find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
         std::basic_ostringstream< TCHAR > os;
         os << dir << _T('\\') << find.cFileName;
         doSearch(os.str().c_str()); //recursive

      } else if ((find.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) != 0) {
         ;
      } else if (! CheckSuffixStage(find.cFileName)) {
         ;
      } else {
         HANDLE hFile = INVALID_HANDLE_VALUE;
         if (CheckSuffixStage(find.cFileName)) {
            //hFile = CreateFile(find.cFileName, GENERIC_READ, FILE_SHARE_READ, 
            //                   NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            int fd;
            errno_t err = _tsopen_s(&fd, find.cFileName,
                                    _O_RDONLY|_O_BINARY, _SH_DENYWR, _S_IREAD|_S_IWRITE);
            if (err == 0) {
               ::pb::Stage* stage = new ::pb::Stage();
               google::protobuf::io::FileInputStream in(fd);
               bool parsed = google::protobuf::TextFormat::Parse(&in, stage);
               in.Close();
               //_close(fd);

               if (parsed && checkStage(stage)) {
                  EvStageSearched e;
                  char* cdir = ::bootes::lib::util::TChar::T2C(dir);
                  char* path = ::bootes::lib::util::TChar::T2C(find.cFileName);
                  e._stage.reset(stage); //own
                  delete[] cdir;
                  delete[] path;
                  g_pFnd->queue(&e);
               } else {
                  delete stage;
               }
            }
         }
      }
   } //for found
   FindClose(hFind);
}

void StageManagerProxy::doSave(const TCHAR* path)
{
   EvSaveStageResult res;
   res._filepath = path;
   res._result = false;

   if (_pStage == NULL) {
      g_pFnd->queue(&res);
      return;
   }

   _pStage->clear_moves();
   _pStage->set_version(1);
   for (MoveSequence::iterator i = _moves.begin(); i != _moves.end(); ++i) {
      pb::Move* pr = IMoveModel::Idealize(*i);
      pr->set_chainnext( _moves.isChainNext(i) );
      _pStage->add_moves()->operator=(*pr);
      delete pr;
   }

   {
      std::basic_string< TCHAR > tc_path, tc_tmp, tc_old;
      tc_path.append(_dir).append(_T("\\")).append(path);
      tc_tmp.append(tc_path).append(_T(".tmp"));
      tc_old.append(tc_path).append(_T(".old"));

      {
         int fd;
         errno_t err = _tsopen_s(&fd, tc_tmp.c_str(), 
                                 _O_WRONLY|_O_BINARY|_O_CREAT|_O_TRUNC, _SH_DENYWR, _S_IREAD|_S_IWRITE);
         if (err != 0) { goto fail; }
      
         google::protobuf::io::FileOutputStream out(fd);
         bool b = google::protobuf::TextFormat::Print(*_pStage, &out);
         out.Flush();
         out.Close();
         //_close(fd);

         if (!b) { goto fail; }
      }

      if (! DeleteFile(tc_old.c_str())) {
         DWORD err = GetLastError();
         if (err != ERROR_FILE_NOT_FOUND) { goto fail; }
      }
      MoveFile(tc_path.c_str(), tc_old.c_str());
      if (! MoveFile(tc_tmp.c_str(), tc_path.c_str())) { goto fail; }

      res._result = true;
      g_pFnd->queue(&res);
   }
   return;

 fail:
   res._result = false;
   g_pFnd->queue(&res);
}

void StageManagerProxy::doLoad(const boost::shared_ptr< ::pb::Stage > stage)
{
   EvLoadStageResult r;
   std::basic_string< TCHAR > moviepath;

   if (stage.operator->() == NULL) { goto fail; }

   {
      TCHAR* tmp = ::bootes::lib::util::TChar::C2T(stage->moviepath().c_str());
      if (tmp == NULL) { goto fail; }
      moviepath = tmp;
      delete[] tmp;
   }

   r._result = false;
   clear(); //_pStage := NULL
   do {
      if (! _pMoviePlayer->load(moviepath.c_str())) { break; }
      r._videoInfo = _pMoviePlayer->getVideoInfo();

      // add move models
      bool chain0 = false;
      for (int i=0; i < stage->moves_size(); ++i) {
         const ::pb::Move& idea = stage->moves(i);
         IMoveModel* rea = IMoveModel::Realize(&idea);
         if (rea) {
            MoveSequence::iterator ite = _moves.add(rea); //pass ownership
            if (chain0) { _moves.chainPrev(ite, true); }
            chain0 = idea.chainnext();
         }
      }
      r._result = true;
   } while(0);
   _pStage = new ::pb::Stage(stage.operator*());

   g_pFnd->queue(&r);
   return;

 fail:
   r._result = false;
   g_pFnd->queue(&r);
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
