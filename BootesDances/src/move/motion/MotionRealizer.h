#ifndef BOOTESDANCES_MOTION_MOTIONREALIZER_H
#define BOOTESDANCES_MOTION_MOTIONREALIZER_H

#include "../../include.h"
#include <bootes/dances/IMotion.h>
#include "Motion.h"
#include "../MoveSequence.h"

class MotionRealizer
{
public:
   typedef std::map< std::string, const MotionRealizer* > Registry;

   static const Registry& GetRegistry();
   static bool Register(MotionRealizer*);
   static const MotionRealizer* GetRealizer(const char* name);

public:
   MotionRealizer();
   virtual ~MotionRealizer();
   virtual const char* getMotionName() const = 0;
   virtual const char* getGuideName() const = 0;
   virtual const TCHAR* getMotionNameT() const = 0;
   virtual const TCHAR* getGuideNameT() const = 0;
   virtual int countSubIds() const = 0;
   virtual IMotion* createMotion(int subid) const = 0;

public:
   static std::basic_string< TCHAR > GetFilePath(const TCHAR* dir, const TCHAR* name, const TCHAR* motion);
   static bool IsExist(const TCHAR* dir, const TCHAR* name, const TCHAR* motion);

   struct MotionData {
      IMotion* pMotion;
      std::string uuid;
      __int64 t0,t1;
   };
   bool save(const TCHAR* dir, const TCHAR* name, const MoveSequence& seq) const;
   bool load(std::list< MotionData >* pOut, const TCHAR* dir, const TCHAR* name) const;
protected:
   virtual bool save(int fd, const MoveSequence&) const = 0;
   virtual bool load(std::list< MotionData >* pOut, int fd) const = 0;
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
