#include "TeachLog.h"
#include <sstream>
#include <iostream>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <io.h> //_close
#include <sys/stat.h>
#include <sys/types.h>
#include <share.h>
#include <fcntl.h>

namespace {

bool SearchLogs(std::map< std::basic_string< TCHAR >, DWORD >* pOut, const TCHAR* basepath, const TCHAR* subdir)
{
   WIN32_FIND_DATA find;
   HANDLE hFind;

   {
      std::basic_string< TCHAR > query;
      query.append(basepath).append(_T("\\")).append(subdir).append(_T("\\teach.log.*"));
      hFind = FindFirstFile(query.c_str(), &find);
      if (hFind == INVALID_HANDLE_VALUE) {
         return true;
      }
   }

   for (BOOL found = TRUE; found; found=FindNextFile(hFind, &find)) {
      pOut->operator[]( std::basic_string< TCHAR >(find.cFileName) ) = find.dwFileAttributes;
   }
   FindClose(hFind);

   return true;
}

bool SearchRegularLogs(std::map< int, std::basic_string< TCHAR > >* pOut, const TCHAR* basepath, const TCHAR* subdir)
{
   std::map< std::basic_string< TCHAR >, DWORD > map;
   if (! SearchLogs(&map, basepath, subdir)) { return false; }

   for (std::map< std::basic_string< TCHAR >, DWORD >::iterator i = map.begin(); i != map.end(); ++i) {
      DWORD attr = i->second;
      if ((attr & FILE_ATTRIBUTE_NORMAL) != 0) { continue; }
      if ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0) { continue; }

      const std::basic_string< TCHAR >& s = i->first;
      if (s.size() < 11) { continue; }
      int n = 0;
      for (int i=10; i<s.size(); ++i) {
         if (s[i] < _T('0') || _T('9') < s[i]) { break; }
         n *= 10;
         n += (s[i] - _T('0'));
      }
      pOut->operator[](n) = s;
   }
   return true;
}

bool GetNewFilepath(std::basic_string< TCHAR >* pOut, const TCHAR* basepath, const TCHAR* subdir)
{
   std::map< std::basic_string< TCHAR >, DWORD > map;
   if (! SearchLogs(&map, basepath, subdir)) { return false; }

   int max = 0;
   for (std::map< std::basic_string< TCHAR >, DWORD >::iterator i = map.begin(); i != map.end(); ++i) {
      const std::basic_string< TCHAR >& s = i->first;
      if (s.size() < 11) { continue; }

      int n = 0;
      for (int i=10; i<s.size(); ++i) {
         if (s[i] < _T('0') || _T('9') < s[i]) { break; }
         n *= 10;
         n += (s[i] - _T('0'));
      }
      if (max < n) { max = n; }
   }

   {
      std::basic_ostringstream< TCHAR > o;
      o << basepath << _T("\\") << subdir << _T("\\teach.log.") << (max+1);
      *pOut = o.str();
   }
   return true;
}

}

bool TeachLogLoader::LoadAll(t_motions* pOut, const IMotion* prototype, const TCHAR* basepath, const TCHAR* subdir)
{
   std::map< int, std::basic_string< TCHAR > > files;
   if (! SearchRegularLogs(&files, basepath, subdir)) { return false; }
   
   // map はキー順、つまりログファイルの番号順にソート済み
   for (std::map< int, std::basic_string< TCHAR > >::iterator i = files.begin(); i != files.end(); ++i) {
      std::basic_string< TCHAR > path;
      path.append(basepath).append(_T("\\")).append(subdir).append(_T("\\")).append(i->second);

      ::pb::TeachLog idea;
      {
         int fd;
         errno_t err = _tsopen_s(&fd, path.c_str(),
                                 _O_RDONLY|_O_BINARY, _SH_DENYWR, _S_IREAD|_S_IWRITE);
         if (err != 0) {
            return false;
         }
         google::protobuf::io::FileInputStream in(fd);
         bool parsed = google::protobuf::TextFormat::Parse(&in, &idea);
         in.Close();
         if (! parsed) {
            return false;
         }
      }
      if (! Realize(pOut, prototype, idea)) {
         return false;
      }
   }
   return true;
}

bool TeachLogLoader::Realize(t_motions* pOut, const IMotion* prototype, const ::pb::TeachLog& in)
{
   for (int i=0; i<in.commands_size(); ++i) {
      const ::pb::TeachCommand& inCmd = in.commands(i);
      if (inCmd.has_clear()) {
         const std::string& uuid = inCmd.clear().uuid();
         t_motions::iterator ite = pOut->find(uuid);
         if (ite != pOut->end()) {
            IMotion* pMotion = ite->second;
            pMotion->teachClear();
         }
         continue;

      } else if (inCmd.has_sequence()) {
         const ::pb::TeachSequence& inSeq = inCmd.sequence();
         const std::string& uuid = inSeq.uuid();
         t_motions::iterator ite = pOut->find(uuid);
         IMotion* pMotion = NULL;
         if (ite != pOut->end()) {
            pMotion = ite->second;
         } else {
            pMotion = prototype->clone();
            pOut->insert( t_motions::value_type(uuid,pMotion) );
         }
         
         pMotion->teachBegin();
         for (int j=0; j<inSeq.records_size(); ++j) {
            const ::pb::TeachRecord& inRecord = inSeq.records(j);

            ::bootes::lib::framework::WiimoteEvent wev;
            wev._accel.t    = inRecord.time();
            wev._accel.x    = inRecord.accel().x();
            wev._accel.y    = inRecord.accel().y();
            wev._accel.z    = inRecord.accel().z();
            wev._gyro.t     = inRecord.time();
            wev._gyro.yaw   = inRecord.gyro().yaw();
            wev._gyro.pitch = inRecord.gyro().pitch();
            wev._gyro.roll  = inRecord.gyro().roll();
            pMotion->teach(inRecord.time(), &wev);
         }
         pMotion->teachCommit(inSeq.succeed());
      }
   }
   return true;
}


TeachLogger::TeachLogger()
{
   _run = false;
   _hThread = INVALID_HANDLE_VALUE;
   _mutex = NULL;
}

TeachLogger::~TeachLogger()
{
   if (_run) {
      stopLater();
      join();
   }
}

DWORD WINAPI TeachLogger::ThreadProc(LPVOID param)
{
   TeachLogger* p = static_cast< TeachLogger* >(param);
   return p->thread_proc();
}

DWORD TeachLogger::thread_proc()
{
   DWORD r = run();

   _hThread = INVALID_HANDLE_VALUE;
   CloseHandle(_mutex);
   _mutex = NULL;

   return r;
}

bool TeachLogger::isRun() const
{
   return (_hThread != INVALID_HANDLE_VALUE);
}

bool TeachLogger::start(const TCHAR* dir)
{
   if (_hThread != INVALID_HANDLE_VALUE) {
      return true;
   }

   _mutex = CreateMutex(NULL, FALSE, NULL);
   if (_mutex == NULL) {
      return false;
   }

   _dir  = dir;
   _hThread = CreateThread(NULL, 0, &TeachLogger::ThreadProc, this, 0, &_thread_id);
   return (_hThread != INVALID_HANDLE_VALUE);
}

void TeachLogger::stopLater()
{
   _run = false;
}

void TeachLogger::join()
{
   if (_hThread == INVALID_HANDLE_VALUE) { return; }
   stopLater();
   WaitForSingleObject(_hThread, INFINITE);
}

DWORD TeachLogger::run()
{
   _run = true;

   std::basic_string< TCHAR > path, subdir;
   int fd = -1;
   errno_t err = 0;
   std::list< Entry* > queue;
   while (_run) {
      if (queue.empty() && !_queue.empty()) {
         WaitForSingleObject(_mutex, INFINITE);
         _queue.swap(queue);
         ReleaseMutex(_mutex);
      }
      if (queue.empty()) {
         Sleep(10);
         continue;
      }

      for (std::list< Entry* >::iterator i=queue.begin(); i!=queue.end(); ++i) {
         Entry* e = *i;
         if (e->command == NULL) {
            if (0 <= fd) {
               _close(fd);
            }
            fd = -1;
            subdir = e->name;

         } else {
            if (fd < 0 && 0 < subdir.size()) {
               if (GetNewFilepath(&path, _dir.c_str(), subdir.c_str())) {
                  err = _tsopen_s(&fd, path.c_str(), 
                                  _O_WRONLY|_O_BINARY|_O_CREAT|_O_EXCL, _SH_DENYWR, _S_IREAD|_S_IWRITE);
                  if (0 < err) {
                     fd = -1;
                  }
               }
               subdir = _T("");
            }
            if (0 < fd) {
               google::protobuf::io::FileOutputStream out(fd);
               ::pb::TeachCommand* p = e->command;
               bool b = google::protobuf::TextFormat::Print(*p, &out);
               out.Flush();
               if (!b) {
                  _close(fd);
                  DeleteFile(path.c_str());
                  fd = -1;
               }
            }
            delete e->command;
         }
         delete e;
      }
      queue.clear();
      Sleep(1);
   }
   
   if (0 < fd) {
      _close(fd);
      fd = -1;
   }

   _run = false;
   WaitForSingleObject(_mutex, INFINITE);
   {
      for (std::list< Entry* >::iterator i=queue.begin(); i!=queue.end(); ++i) {
         Entry* e = *i;
         if (e->command) {
            delete e->command;
         }
         delete e;
      }
      for (std::list< Entry* >::iterator i=_queue.begin(); i!=_queue.end(); ++i) {
         Entry* e = *i;
         if (e->command) {
            delete e->command;
         }
         delete e;
      }
   }
   ReleaseMutex(_mutex);
   return 0;
}

bool TeachLogger::open(const TCHAR* subdir)
{
   bool ret = false;
   if (!_run) { return false; }
   WaitForSingleObject(_mutex, INFINITE);
   {
      if (_run) {
         _queue.push_back( new Entry(NULL, subdir) );
         ret = true;
      }
   }
   ReleaseMutex(_mutex);
   return ret;
}

bool TeachLogger::add(::pb::TeachCommand* p)
{
   bool ret = false;
   if (!_run) { goto fail; }
   WaitForSingleObject(_mutex, INFINITE);
   {
      if (_run) {
         _queue.push_back( new Entry(p, _T("")) );
         ret = true;
      }
   }
   ReleaseMutex(_mutex);
   return ret;

fail:
   delete p;
   return false;
}

bool TeachLogger::add_sequence(const std::string& uuid, const TeachSequence& seq)
{
   ::pb::TeachCommand* p = new ::pb::TeachCommand();
   ::pb::TeachSequence* pSeq = p->mutable_sequence();
   pSeq->set_uuid(uuid);
   pSeq->set_succeed(seq.succeed);
   for (TeachSequence::t_records::const_iterator i = seq.records.begin(); i != seq.records.end(); ++i) {
      const TeachSequence::Record& r = *i;
      ::pb::TeachRecord* pRecord = pSeq->add_records();
      pRecord->set_time(r.time);
      pRecord->mutable_accel()->set_x(r.wiimote._accel.x);
      pRecord->mutable_accel()->set_y(r.wiimote._accel.y);
      pRecord->mutable_accel()->set_z(r.wiimote._accel.z);
      pRecord->mutable_gyro()->set_yaw(r.wiimote._gyro.yaw);
      pRecord->mutable_gyro()->set_pitch(r.wiimote._gyro.pitch);
      pRecord->mutable_gyro()->set_roll(r.wiimote._gyro.roll);
   }
   return add(p);
}

bool TeachLogger::add_clear(const std::string& uuid)
{
   ::pb::TeachCommand* p = new ::pb::TeachCommand();
   ::pb::TeachClear* pClear = p->mutable_clear();
   pClear->set_uuid(uuid);
   return add(p);
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
