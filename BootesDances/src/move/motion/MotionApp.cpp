#include "MotionApp.h"
#include <stdio.h>
#include <cstdarg>
#include <tchar.h>
#include <wchar.h>
#include <locale.h>
#include <errno.h>
#include <windows.h>
#include <d3dx9.h>

int main(int argc, char** argv)
{
   MotionApp app;
   MotionAppOption opt;
   opt.parse(argc, argv);

   return app.main(opt);
}

D3DXVECTOR4* WINAPI D3DXVec3TransformArray(D3DXVECTOR4*, UINT, const D3DXVECTOR3*, UINT, const D3DXMATRIX*, UINT)
{ return NULL; }
D3DXMATRIX* WINAPI D3DXMatrixTranslation(D3DXMATRIX*, FLOAT, FLOAT, FLOAT)
{ return NULL; }


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

   {
      const char* gname = _pMotionRealizer->getGuideName();
      const GuideRealizer* pGR = GuideRealizer::GetRealizer(gname);
      if (pGR == NULL) {
         help("no guide specified by motion algorithm: \"%s\" by \"%s\"", gname, algorithm.c_str());
      }
      _pGuideRealizer = pGR;
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
   for (t_moves::iterator i = _moves.begin(); i != _moves.end(); ++i) {
      delete i->second;
   }
   _moves.clear();
}

int MotionApp::main(const MotionAppOption& opt)
{
   std::basic_string< TCHAR > dir;
   {
      TCHAR* p = C2T(opt._dir.c_str());
      dir.append(p);
      delete[] p;
   }

   if (! loadTeachLog(dir.c_str())) {
      printf("fail to load logfile in dir of \"%s\"\n", opt._dir.c_str());
      return 1;
   }

   if (! loadGuide(opt._pGuideRealizer, dir.c_str())) {
      printf("fail to load guidefile in dir of \"%s\"\n", opt._dir.c_str());
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

bool MotionApp::loadTeachLog(const TCHAR* dir)
{
   std::list< ::pb::TeachLog > logs;
   if (! TeachLogLoader::LoadAll(&logs, dir, NULL)) {
      return false;
   }
   if (logs.empty()) {
      printf("no logfile loaded\n");
      return false;
   }

   TeachLogLoader::Merge(&_log, logs, true);

   return true;
}

bool MotionApp::loadGuide(const GuideRealizer* pGuideRealizer, const TCHAR* dir)
{
   std::list< GuideRealizer::GuideData > guides;
   if (! pGuideRealizer->load(&guides, dir, NULL)) {
      return false;
   }
   for (std::list< GuideRealizer::GuideData >::iterator i = guides.begin(); i != guides.end(); ++i) {
      GuideRealizer::GuideData& d = *i;
      t_moves::iterator mi = _moves.find(d.uuid);
      if (mi == _moves.end()) {
         Move* pMove = new Move();
         pMove->setGuide(d.pGuide);
         pMove->setTime(d.t0, d.t1);
         pMove->setUuid(d.uuid.c_str());
         _moves[d.uuid] = pMove;
      }
   }
   return true;
}

bool MotionApp::teach(const MotionRealizer* pRealizer, int subid)
{
   for (int i=0; i<_log.commands_size(); ++i) {
      const ::pb::TeachCommand& inCmd = _log.commands(i);
      const std::string& uuid = inCmd.uuid();
      Move* pMove = NULL;
      {
         t_moves::iterator ite = _moves.find(uuid);
         if (ite == _moves.end()) {
            continue;
         }
         pMove = ite->second;
      }

      if (pMove->getMotion() == NULL) {
         IMotion* pMotion = pRealizer->createMotion(subid);
         if (pMotion == NULL) {
            printf("fail to realizer Motion");
            return false;
         }
         pMove->setMotion(pMotion);
      }
      IMotion* pMotion = pMove->getMotion();

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
   for (t_moves::iterator mi = _moves.begin(); mi != _moves.end(); ++mi) {
      const std::string& uuid = mi->first;
      IMotion* pMotion = mi->second->getMotion();

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
         //printf("%s: -/0\n", uuid.c_str());
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
