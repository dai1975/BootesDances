#ifndef BOOTESDANCES_GUIDE_GUIDEREALIZER_H
#define BOOTESDANCES_GUIDE_GUIDEREALIZER_H

#include "../../include.h"
#include <bootes/dances/IGuide.h>
#include "Guide.h"

class GuideRealizer
{
public:
   static bool Idealize(::pb::Guide*, const IGuide*);
   static bool Realize(IGuide**, const ::pb::Guide*);
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
