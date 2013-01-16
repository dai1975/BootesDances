#ifndef BOOTESDANCES_VIEW_ROOTVIEW_H
#define BOOTESDANCES_VIEW_ROOTVIEW_H

#include "../include.h"
#include <bootes/dances/IStageManager.h>

class RootView: public ::bootes::lib::framework::View
{
 public:
   virtual IStageManager* getStageManager() = 0;
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
