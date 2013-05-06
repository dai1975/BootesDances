#ifndef BOOTESDANCES_MOTION_MOTIONAPP_H
#define BOOTESDANCES_MOTION_MOTIONAPP_H

#include "TeachLog.h"
#include "MotionRealizer.h"
#include "../Move.h"
#include "../guide/GuideRealizer.h"
#include <list>
#include <string>

class MotionAppOption
{
public:
   MotionAppOption();
   void parse(int& argc, char**& argv);
   void help(const char* fmt, ...);
   
   const char* _arg0;
   const MotionRealizer* _pMotionRealizer;
   const GuideRealizer* _pGuideRealizer;
   int _motion_subid;
   std::string _dir;
};

class MotionApp
{
public:
   MotionApp();
   ~MotionApp();

   int main(const MotionAppOption& opt);
   bool loadTeachLog(const TCHAR* dir);
   bool loadGuide(const GuideRealizer* pGuideRealizer, const TCHAR* dir);
   bool teach(const MotionRealizer* pRealizer, int subid);
   bool test();

private:
   ::pb::TeachLog _log;

   typedef std::map< std::string, Move* > t_moves;
   t_moves _moves;
};

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
