#include "MotionApp.h"
#include <stdio.h>
#include <cstdarg>
#include <tchar.h>
#include <wchar.h>
#include <locale.h>
#include <errno.h>

int main(int argc, char** argv)
{
   MotionApp app;
   MotionAppOption opt;
   opt.parse(argc, argv);

   return app.main(opt);
}

namespace {

#if defined(_UNICODE)
TCHAR*   C2T(const  char* src) {
   size_t wsize;
   if (mbstowcs_s(&wsize, NULL, 0, src, 0) != 0) {
      return NULL;
   }
   wchar_t* ret = new wchar_t[wsize];
   if (mbstowcs_s(&wsize, ret, wsize, src, wsize) != 0) {
      delete[] ret;
      return NULL;
   }
   return ret;
}

#else
TCHAR*   C2T(const  char* src) {
   size_t len  = strlen(src) + 1;
   char* r = new char[len];
   memcpy(r, src, sizeof(char)*len);
   return r;
}
#endif
}


MotionAppOption::MotionAppOption()
{
   _arg0 = "";
   _pMotionRealizer = NULL;
}

void MotionAppOption::help(const char* fmt, ...)
{
   va_list va;
   va_start(va, fmt);
   if (fmt != NULL && *fmt != '\0') {
      vprintf(fmt, va);
      printf("\n");
   }
   va_end(va);

   printf("%s [options] <stage dir>\n", _arg0);
   printf("options:\n");
   printf("  -h                     show this message.\n");
   printf("  -a <algorithm>[:subid] specify motion algorithm. default subid is 0.\n");
   printf("  algorithm are:\n");
   {
      printf("    ");
      const MotionRealizer::Registry& r = MotionRealizer::GetRegistry();
      for (MotionRealizer::Registry::const_iterator i = r.begin(); i != r.end(); ++i) {
         if (i != r.begin()) { printf(", "); }
         printf("%s", i->first.c_str());
         int nSubIds = i->second->countSubIds();
         if (nSubIds == 1) {
            printf("[0]");
         } else {
            printf("[0..%d]", nSubIds-1);
         }
      }
      printf("\n");
   }
   std::exit(0);
}

void MotionAppOption::parse(int& argc, char**& argv)
{
   bool bHelp = false;
   std::string algorithm;

   _arg0 = *argv;
   for (--argc; argc; --argc) {
      ++argv;
      if (**argv == '-') {
         switch (*++*argv) {
         case 'h':
            bHelp = true;
            break;
         case 'a':
            if (--argc == 0) { help("need algorithm name"); }
            algorithm = *++argv;
            break;
         default:
            help("unknown option \"-%s\"", *argv);
         }
      } else {
         break;
      }
   }

   if (bHelp) { help(""); }

   {
      if (algorithm.empty()) { help("no algorithm"); }
      int subid = 0;
      {
         int pos = algorithm.rfind(":");
         if (pos != std::string::npos) {
            for (int i=pos+1; i<algorithm.size(); ++i) {
               if (algorithm[i] < '0' || '9' < algorithm[i]) {
                  help("malformed subid: \"%s\"", algorithm.c_str());
               }
               subid *= 10;
               subid += (algorithm[i] - '0');
            }
            algorithm = algorithm.substr(0, pos);
         }
      }
      _motion_subid = subid;

      {
         const MotionRealizer::Registry& r = MotionRealizer::GetRegistry();
         MotionRealizer::Registry::const_iterator i = r.find(algorithm);
         if (i == r.end()) {
            help("unknown algorithm: \"%s\"", algorithm.c_str());
         }
         _pMotionRealizer = i->second;
      }
   }

   if (argc == 0) { help("no directory"); }
   --argc;
   _dir = std::string(*argv++);
}

//------------------------------------------------------------------------------------
MotionApp::MotionApp()
{
}

MotionApp::~MotionApp()
{
   for (t_motions::iterator i = _motions.begin(); i != _motions.end(); ++i) {
      delete i->second;
   }
   _motions.clear();
}

int MotionApp::main(const MotionAppOption& opt)
{
   if (! load(opt._dir.c_str())) {
      printf("fail to load logfile in dir of \"%s\"\n", opt._dir.c_str());
      return 1;
   }

   if (! teach(opt._pMotionRealizer, opt._motion_subid)) {
      printf("fail to teach: %s:%d\n", opt._pMotionRealizer->getMotionName(), opt._motion_subid);
      return 2;
   }

   if (! test()) {
      printf("fail to test\n");
      return 3;
   }
   
   return 0;
}

bool MotionApp::load(const char* cdir)
{
   std::basic_string< TCHAR > dir;
   {
      TCHAR* p = C2T(cdir);
      dir.append(p);
      delete[] p;
   }

   std::list< ::pb::TeachLog > logs;
   if (! TeachLogLoader::LoadAll(&logs, dir.c_str(), NULL)) {
      return false;
   }
   if (logs.empty()) {
      printf("no logfile loaded\n");
      return false;
   }

   TeachLogLoader::Merge(&_log, logs, true);

   return true;
}

bool MotionApp::teach(const MotionRealizer* pRealizer, int subid)
{
   for (int i=0; i<_log.commands_size(); ++i) {
      const ::pb::TeachCommand& inCmd = _log.commands(i);
      const std::string& uuid = inCmd.uuid();
      IMotion* pMotion = NULL;
      {
         t_motions::iterator ite = _motions.find(uuid);
         if (ite == _motions.end()) {
            pMotion = pRealizer->createMotion(subid);
            if (pMotion == NULL) {
               printf("fail to realizer Motion");
               return false;
            }
            _motions[uuid] = pMotion;
         } else {
            pMotion = ite->second;
         }
      }

      if (inCmd.has_clear()) {
         pMotion->teachClear();

      } else if (inCmd.has_sequence()) {
         const ::pb::TeachSequence& inSeq = inCmd.sequence();
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

bool MotionApp::test()
{
   for (t_motions::iterator mi = _motions.begin(); mi != _motions.end(); ++mi) {
      const std::string& uuid = mi->first;
      IMotion* pMotion = mi->second;

      int total   = 0;
      int succeed = 0;
      int failed  = 0;
      for (int li=0; li<_log.commands_size(); ++li) {
         const ::pb::TeachCommand& inCmd = _log.commands(li);
         if (uuid != inCmd.uuid()) { continue; }

         if (inCmd.has_clear()) {
            continue;
         } else if (inCmd.has_sequence()) {
            const ::pb::TeachSequence& inSeq = inCmd.sequence();
            pMotion->testBegin();
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
               pMotion->test(inRecord.time(), &wev);
            }
            pMotion->testEnd(true);
            ++total;
            switch (pMotion->getTestState()) {
            case IMotion::TEST_SUCCEED:
               ++succeed;
               break;
            case IMotion::TEST_FAILED:
               ++failed;
               break;
            }
         } //seq
      } //log

      if (total == 0) {
         printf("%s: -/0\n", uuid.c_str());
      } else {
         printf("%s: %d/%d(%.2f%%)\n", uuid.c_str(), succeed, total, ((100.f * succeed)/total));
      }
   }
   return true;
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
