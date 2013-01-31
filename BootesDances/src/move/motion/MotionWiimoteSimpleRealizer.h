#ifndef BOOTESDANCES_MOTION_WIIMOTESIMPLEREALIZER_H
#define BOOTESDANCES_MOTION_WIIMOTESIMPLEREALIZER_H

#include "MotionFactory.h"

class MotionWiimoteSimpleRealizer: public MotionFactory
{
public:
   virtual const char* getGuideName() const;
   virtual int countSubIds() const;
   virtual const char* getMotionName() const;
   virtual IMotion* createMotion(int subid) const;
   virtual bool save(::google::protobuf::io::ZeroCopyOutputStream& out, const MoveSequence*) const;
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
