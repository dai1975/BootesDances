#include "include.h"
#include <bootes/lib/booteslib.h>
#include "framework/FoundationImpl.h"

namespace bootes { namespace lib {

::bootes::lib::framework::Foundation* g_pFnd = NULL;

::bootes::lib::framework::Foundation* GetFoundation() { return g_pFnd; }

bool Initialize(Initializer* ini)
{
   if (g_pFnd) { return false; }

   ::bootes::lib::framework::Foundation* pFoundation = NULL;

   pFoundation = ini->createFoundation();
   if (pFoundation == NULL) {
      goto fail;
   }

   g_pFnd = pFoundation;
   return true;

fail:
   if (pFoundation) { delete pFoundation; }
   return false;
}

void Finalize()
{
   if (g_pFnd) { delete g_pFnd; g_pFnd = NULL; }
}

} }

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
