#ifndef BOOTES_LIB_TIMER_H
#define BOOTES_LIB_TIMER_H

#include "macros.h"
#include "../win.h"

namespace bootes { namespace lib { namespace util {

class Timer
{
public:
   Timer();
   ~Timer();

   void reset(); //!< reset counter. pausing state is not changed.
   void start(); //!< start counting.
   void pause(); //!< stop counting.

   /**!
    * total:   total time[msec] from reset or first start except being paused.
    * elapsed: time[msec] from the last start/reset/pause/get.
    */
   void get(double* total, int* elapsed);

private:
   bool   m_pause;
   DWORD  m_last;
   double m_total;
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
