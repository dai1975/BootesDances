#ifndef BOOTESDANCES_INCLUDE_H
#define BOOTESDANCES_INCLUDE_H

#if defined(_MSC_VER) && (1600 <= _MSC_VER)
#include <intsafe.h>
#pragma warning(push)
#pragma warning(disable:4005)
#include <stdint.h>
#pragma warning(pop)
#endif

#include <limits.h>
#include <assert.h>

#include <bootes/lib/booteslib.h>
#include <bootes/dances/IBootesGame.h>

extern ::bootes::lib::framework::Foundation* g_pFnd;
extern IBootesGame* g_pGame;

#define FLOAT_PI 3.141592f

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
