#ifndef BOOTESDANCES_IBOOTESGAME_H
#define BOOTESDANCES_IBOOTESGAME_H

#include <bootes/lib/booteslib.h>
#include "IStageManager.h"

class IBootesGame: public ::bootes::lib::framework::Game
{
 public:
   virtual IStageManager* getStageManager() = 0;
   virtual const TCHAR* getUserDir() const = 0;
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
