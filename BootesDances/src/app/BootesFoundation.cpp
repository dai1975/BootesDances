#include "BootesFoundation.h"
#include <bootes/dances/BootesWiimote.h>

::bootes::lib::framework::Wiimote* BootesFoundation::createWiimote() const
{
   return new BootesWiimote();
}

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
