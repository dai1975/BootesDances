#ifndef BOOTES_APP_BOOTESFOUNDATION_H
#define BOOTES_APP_BOOTESFOUNDATION_H

#include "../include.h"
#include <bootes/lib/framework/Foundation.h>
#include <bootes/lib/framework/InputEvent.h>

class BootesFoundation: public bootes::lib::framework::Foundation
{
   typedef bootes::lib::framework::Foundation super;

protected:
   ::bootes::lib::framework::Wiimote* createWiimote() const;
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
