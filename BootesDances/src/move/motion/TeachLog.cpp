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

bool SearchLogs(std::map< std::basic_string< TCHAR >, DWORD >* pOut, const TCHAR* dirpath)
{
   WIN32_FIND_DATA find;
   HANDLE hFind;

   {
      std::basic_string< TCHAR > query;
      query.append(dirpath).append(_T("\\teach.log.*"));
      hFind = FindFirstFile(query.c_str(), &find);
      if (hFind == INVALID_HANDLE_VALUE) {
         return true;
      }
   }

   for (BOOL found = TRUE; found; found=FindNextFile(hFind, &find)) {
      std::basic_string< TCHAR > s;
      s.append(dirpath).append(_T("\\")).append(find.cFileName);
      pOut->operator[](s) = find.dwFileAttributes;
   }
   FindClose(hFind);

   return true;
}

int ParseLogIndex(const std::basic_string<TCHAR> filename)
{
   if (filename.empty()) { return -1; }

   int i;
   for (i = filename.size()-1; 0 <= i && filename[i] != _T('.'); --i) {
      if (filename[i] < _T('0') || _T('9') < filename[i]) {
         return -1;
      }
   }
   if (i < 0 || filename[i] != _T('.')) { return -1; }

   int n = 0;
   for (++i; i<filename.size(); ++i) {
      n *= 10;
      n += (filename[i] - _T('0'));
   }
   return n;
}

bool SearchRegularLogs(std::map< int, std::basic_string< TCHAR > >* pOut, const TCHAR* dirpath)
{
   std::map< std::basic_string< TCHAR >, DWORD > map;
   if (! SearchLogs(&map, dirpath)) { return false; }

   for (std::map< std::basic_string< TCHAR >, DWORD >::iterator i = map.begin(); i != map.end(); ++i) {
      DWORD attr = i->second;
      if ((attr & FILE_ATTRIBUTE_NORMAL) != 0) { continue; }
      if ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0) { continue; }

      const std::basic_string< TCHAR >& s = i->first;
      int n = ParseLogIndex(s);
      if (0 <= n) {
         pOut->operator[](n) = s;
      }
   }
   return true;
}

bool GetNewFilepath(std::basic_string< TCHAR >* pOut, const TCHAR* dirpath)
{
   std::map< std::basic_string< TCHAR >, DWORD > map;
   if (! SearchLogs(&map, dirpath)) { return false; }

   int max = 0;
   for (std::map< std::basic_string< TCHAR >, DWORD >::iterator i = map.begin(); i != map.end(); ++i) {
      const std::basic_string< TCHAR >& s = i->first;
      int n = ParseLogIndex(s);
      if (max < n) { max = n; }
   }

   {
      std::basic_ostringstream< TCHAR > o;
      o << dirpath << _T("\\teach.log.") << (max+1);
      *pOut = o.str();
   }
   return true;
}

}

bool TeachLogLoader::LoadAll(std::list< ::pb::TeachLog >* pOut, const TCHAR* basepath, const TCHAR* subdir)
{
   std::basic_string< TCHAR > dirpath;
   if (subdir && *subdir != _T('\0')) {
      dirpath.append(basepath).append(_T("\\")).append(subdir);
   } else {
      dirpath.append(basepath);
   }

   std::map< int, std::basic_string< TCHAR > > files;
   if (! SearchRegularLogs(&files, dirpath.c_str())) { return false; }
   
   // map はキー順、つまりログファイルの番号順にソート済み
   for (std::map< int, std::basic_string< TCHAR > >::iterator i = files.begin(); i != files.end(); ++i) {
      std::basic_string< TCHAR >& filepath = i->second;

      ::pb::TeachLog iLog;
      {
         int fd;
         errno_t err = _tsopen_s(&fd, filepath.c_str(),
                                 _O_RDONLY|_O_BINARY, _SH_DENYWR, _S_IREAD|_S_IWRITE);
         if (err != 0) {
            return false;
         }

         google::protobuf::io::FileInputStream in(fd);
         bool parsed = google::protobuf::TextFormat::Parse(&in, &iLog);
         if (!parsed) {
            in.Close();
            return false;
         }
         in.Close();
      }
      pOut->push_back(iLog);
   }
   return true;
}

void TeachLogLoader::Merge(::pb::TeachLog* pOut, const std::list< ::pb::TeachLog >& ins, bool strip)
{
   std::map< std::string, std::pair<int,int> > uuid2lastindex;
   int insidx = 0;
   if (strip) {
      for (std::list< ::pb::TeachLog >::const_iterator insite = ins.begin(); insite != ins.end(); ++insite, ++insidx) {
         const ::pb::TeachLog& in = *insite;
         for (int i=0; i<in.commands_size(); ++i) {
            const ::pb::TeachCommand& inCmd = in.commands(i);
            const std::string& uuid = inCmd.uuid();
            if (inCmd.has_clear()) {
               uuid2lastindex[uuid] = std::pair<int,int>(insidx, i);
            }
         }
      }
   }

   insidx = 0;
   for (std::list< ::pb::TeachLog >::const_iterator insite = ins.begin(); insite != ins.end(); ++insite, ++insidx) {
      const ::pb::TeachLog& in = *insite;
      for (int i=0; i<in.commands_size(); ++i) {
         const ::pb::TeachCommand& inCmd = in.commands(i);
         bool skip = false;
         if (strip) {
            const std::string& uuid = inCmd.uuid();
            if (inCmd.has_clear()) {
               skip = true;
            } else {
               std::map< std::string, std::pair<int,int> >::iterator j = uuid2lastindex.find(uuid);
               if (j != uuid2lastindex.end()) {
                  if (insidx < j->second.first) {
                     skip = true;
                  } else if (insidx == j->second.first && i <= j->second.second) {
                     skip = true;
                  }
               }
            }
         }
         if (! skip) {
            pOut->add_commands()->operator=(inCmd);
         }
      }
   }
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

   std::basic_string< TCHAR > filepath, subdir;
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
               std::basic_string<TCHAR> dirpath;
               dirpath.append(_dir).append(_T("\\")).append(subdir);
               if (GetNewFilepath(&filepath, dirpath.c_str())) {
                  err = _tsopen_s(&fd, filepath.c_str(), 
                                  _O_WRONLY|_O_BINARY|_O_CREAT|_O_EXCL, _SH_DENYWR, _S_IREAD|_S_IWRITE);
                  if (0 < err) {
                     fd = -1;
                  }
               }
               subdir = _T("");
            }
            if (0 < fd) {
               google::protobuf::io::FileOutputStream out(fd);
               //::pb::TeachCommandHolder holder;
               //holder.mutable_command()->operator=(*e->command);
               ::pb::TeachLog tmp;
               tmp.add_commands()->operator=(*e->command);
               bool b = google::protobuf::TextFormat::Print(tmp, &out);
               out.Flush();
               if (!b) {
                  _close(fd);
                  DeleteFile(filepath.c_str());
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
   p->set_uuid(uuid);
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
   p->set_uuid(uuid);
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
