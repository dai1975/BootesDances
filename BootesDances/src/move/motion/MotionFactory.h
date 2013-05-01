#error
#ifndef BOOTESDANCES_MOTION_FACTORY_H
#define BOOTESDANCES_MOTION_FACTORY_H

#include <bootes/dances/IMotion.h>
#include "../MoveSequence.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

class MotionFactory
{
public:
   typedef std::map< std::string, const MotionFactory* > Registry;

   static const Registry& GetRegistry();
   static bool Register(MotionFactory*);
   static const MotionFactory* GetFactory(const char* name);

public:
   virtual inline ~MotionFactory() { }
   virtual const char* getMotionName() const = 0;
   virtual const char* getGuideName() const = 0;
   virtual const TCHAR* getMotionNameT() const = 0;
   virtual const TCHAR* getGuideNameT() const = 0;
   virtual int countSubIds() const = 0;
   virtual IMotion* createMotion(int subid) const = 0;
   virtual bool save(::google::protobuf::io::ZeroCopyOutputStream& out, const MoveSequence&) const = 0;
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
