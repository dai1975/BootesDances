#ifndef BOOTES_LIB_FRAMEWORK_VIEW_H
#define BOOTES_LIB_FRAMEWORK_VIEW_H

#include "macros.h"
#include "InputEvent.h"

namespace bootes { namespace lib { namespace framework {

class View
{
public:
   inline View() { }
	inline virtual ~View() { }

public:
	virtual void onUpdate(double currentTime, int elapsedTime) = 0;
	virtual void onRender(double currentTime, int elapsedTime) = 0;
	virtual bool onInput(const InputEvent*) = 0;
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
