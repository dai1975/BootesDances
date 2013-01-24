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

bool GetFilepathes(std::map< int, std::basic_string< TCHAR > >* pOut, const TCHAR* dir, const TCHAR* name)
{
   WIN32_FIND_DATA find;
   HANDLE hFind;

   {
      std::basic_string< TCHAR > query = dir;
      query += _T("\\");
      query += name;
      query += _T("-*.teach");
      hFind = FindFirstFile(query.c_str(), &find);
      if (hFind == INVALID_HANDLE_VALUE) {
         return true;
      }
   }

   for (BOOL found = TRUE; found; found=FindNextFile(hFind, &find)) {
      if ((find.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) != 0) { continue; }
      int n = 0;
      for (size_t i=0; find.cFileName[i] != _T('.'); ++i) {
         if (_T('0') <= find.cFileName[i] && find.cFileName[i] <= _T('9')) {
            n *= 10;
            n += find.cFileName[i] - _T('0');
         } else {
            n = -1;
            break;
         }
      }
      if (0 <= n) {
         std::basic_string< TCHAR >& s = pOut->operator[](n);
         s = dir;
         s += _T("\\");
         s += find.cFileName;
      }
   }
   FindClose(hFind);

   return true;
}

bool GetNewFilepath(std::basic_string< TCHAR >* pOut, const TCHAR* dir, const TCHAR* name)
{
   std::map< int, std::basic_string< TCHAR > > map;
   if (! GetFilepathes(&map, dir, name)) { return false; }

   int max = 0;
   if (! map.empty()) {
      std::map< int, std::basic_string< TCHAR > >::iterator i;
      i = map.end();
      --i;
      max = i->first;
   }

   {
      std::basic_ostringstream< TCHAR > o;
      o << dir << _T("\\") << name << _T("-") << (max+1) << _T(".teach");
      *pOut = o.str();
   }
   return true;
}

}

bool TeachLogLoader::Load(t_motions* pOut, const IMotion* prototype, const TCHAR* dir, const TCHAR* name)
{
   std::map< int, std::basic_string< TCHAR > > pathes;
   if (! GetFilepathes(&pathes, dir, name)) { return false; }
   
   for (std::map< int, std::basic_string< TCHAR > >::iterator i = pathes.begin(); i != pathes.end(); ++i) {
      int num = i->first;
      std::basic_string< TCHAR >& path = i->second;

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

   std::basic_string< TCHAR > path, name;
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
            name = e->name;

         } else {
            if (fd < 0 && 0 < name.size()) {
               if (GetNewFilepath(&path, _dir.c_str(), name.c_str())) {
                  err = _tsopen_s(&fd, path.c_str(), 
                                  _O_WRONLY|_O_BINARY|_O_CREAT|_O_EXCL, _SH_DENYWR, _S_IREAD|_S_IWRITE);
                  if (0 < err) {
                     fd = -1;
                  }
               }
               name = _T("");
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

bool TeachLogger::open(const TCHAR* name)
{
   bool ret = false;
   if (!_run) { return false; }
   WaitForSingleObject(_mutex, INFINITE);
   {
      if (_run) {
         _queue.push_back( new Entry(NULL, name) );
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
