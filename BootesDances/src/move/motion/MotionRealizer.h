#ifndef BOOTESDANCES_MOTION_MOTIONREALIZER_H
#define BOOTESDANCES_MOTION_MOTIONREALIZER_H

#include "../../include.h"
#include <bootes/dances/IMotion.h>
#include "Motion.h"

class MotionRealizer
{
public:
   static bool Idealize(::pb::Motion*, const IMotion*);
   static bool Realize(IMotion**, const ::pb::Motion*);
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
