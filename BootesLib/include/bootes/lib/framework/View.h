#ifndef BOOTES_LIB_FRAMEWORK_VIEW_H
#define BOOTES_LIB_FRAMEWORK_VIEW_H

#include "macros.h"
#include "GameTime.h"
#include "InputEvent.h"

namespace bootes { namespace lib { namespace framework {

class View
{
protected:
   typedef ::bootes::lib::framework::InputEvent InputEvent;
   typedef ::bootes::lib::framework::GameTime   GameTime;

public:
   inline View() { }
   inline virtual ~View() { }

public:
   virtual void onUpdate(const GameTime* gt) = 0;
   virtual void onRender(const GameTime* gt) = 0;
   virtual bool onInput(const GameTime* gt, const InputEvent*) = 0;
   virtual bool onSensorInput(const GameTime* gt, const InputEvent*) = 0;
   virtual void onLostDevice() = 0;
   virtual void onResetDevice() = 0;
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
