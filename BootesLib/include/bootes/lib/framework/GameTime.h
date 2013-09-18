#ifndef BOOTES_LIB_FRAMEWORK_GAMETIME_H
#define BOOTES_LIB_FRAMEWORK_GAMETIME_H

#include "macros.h"

namespace bootes { namespace lib { namespace framework {

struct GameTime
{
public:
   double total;   //msec
   int    elapsed; //msec
};

} } }

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
