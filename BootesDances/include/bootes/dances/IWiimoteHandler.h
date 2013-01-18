#ifndef BOOTESDANCES_STAGE_IWIIMOTE_HANDLER_H
#define BOOTESDANCES_STAGE_IWIIMOTE_HANDLER_H

#include <bootes/lib/framework/InputEvent.h>
#include "Scene.h"

class IWiimoteHandler
{
public:
   virtual void teachClear(const IMove*) = 0;

   virtual void teachBegin(const IMove*) = 0;
   virtual void teachCommit(bool succeed) = 0;
   virtual void teachRollback() = 0;

   virtual void handleWiimote(const Scene*, const ::bootes::lib::framework::WiimoteEvent*) = 0;
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
