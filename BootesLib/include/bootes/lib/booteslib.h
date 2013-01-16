#ifndef BOOTES_LIB_BOOTESLIB_H
#define BOOTES_LIB_BOOTESLIB_H

#include "macros.h"
#include "win.h"
#include <d3dx9.h>

#include "framework.h"
#include "util/macros.h"

namespace bootes { namespace lib {

class Initializer
{
public:
   virtual ::bootes::lib::framework::Foundation* createFoundation() = 0;
};

bool Initialize(Initializer*);
void Finalize();

::bootes::lib::framework::Foundation* GetFoundation();

} }

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
