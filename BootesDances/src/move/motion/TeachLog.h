#ifndef BOOTESDANCES_MOTION_TEACHLOG_H
#define BOOTESDANCES_MOTION_TEACHLOG_H

#include "../../include.h"
#include <bootes/dances/proto/Teach.pb.h>
#include <bootes/dances/IMotion.h>
#include "Teach.h"

class TeachLogger
{
public:
   TeachLogger();
   ~TeachLogger();

   static DWORD WINAPI ThreadProc(LPVOID param);
   bool isRun() const;
   bool start(const TCHAR* dir);
   void stopLater();
   void join();

   bool open(const TCHAR* name);
   bool add_sequence(const std::string& uuid, const TeachSequence&);
   bool add_clear(const std::string& uuid);

private:
   bool add(::pb::TeachCommand*);
   DWORD thread_proc();
   DWORD run();

   bool _run;
   HANDLE _hThread;
   DWORD _thread_id;
   HANDLE _mutex;
   std::basic_string< TCHAR > _dir; 

   struct Entry {
      ::pb::TeachCommand* command;
      std::basic_string< TCHAR > name;
      inline Entry(): command(NULL) { }
      inline Entry(::pb::TeachCommand* c, const TCHAR* n): command(c), name(n) { }
   };
   std::list< Entry* > _queue;
};

class TeachLogLoader
{
public:
   typedef std::map< std::string, IMotion* > t_motions;
   static bool LoadAll(t_motions* pOut, const IMotion* prototype, const TCHAR* dir, const TCHAR* name);
   static bool Realize(t_motions* pOut, const IMotion* prototype, const ::pb::TeachLog& idea);
};

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
