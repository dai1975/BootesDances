#ifndef BOOTESDANCES_MOTION_WIIMOTESIMPLEREALIZER_H
#define BOOTESDANCES_MOTION_WIIMOTESIMPLEREALIZER_H

#include "MotionRealizer.h"

class MotionWiimoteSimpleRealizer: public MotionRealizer
{
public:
   virtual const char* getGuideName() const;
   virtual const char* getMotionName() const;
   virtual const TCHAR* getGuideNameT() const;
   virtual const TCHAR* getMotionNameT() const;
   virtual int countSubIds() const;
   virtual IMotion* createMotion(int subid) const;

protected:
   virtual bool save(int fd, const MoveSequence&) const;
   virtual bool load(std::list< MotionData >* pOut, int fd) const;
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
