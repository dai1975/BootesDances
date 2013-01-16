#include "../include.h"
#include <bootes/lib/util/Timer.h>
#include <mmsystem.h>

namespace bootes { namespace lib { namespace util {

Timer::Timer()
{
   m_total = 0;
   m_last = 0;
   m_pause = false;
}

Timer::~Timer()
{
}

void Timer::reset()
{
   m_total = 0;
   m_last = timeGetTime();
}

void Timer::get(double* total, int* elapsed)
{
   DWORD t = timeGetTime(); //msec
   int dt = (m_last <= t)
      ? (t - m_last)
      : t + (ULONG_MAX - m_last);

   if (! m_pause) {
      m_last = t;
      m_total += dt;
   }
   
   if (elapsed) { *elapsed = dt; }
   if (total) { *total = m_total; }
}

void Timer::start()
{
   DWORD t = timeGetTime();
   m_last = t;
   m_pause = false;
}

void Timer::pause()
{
   if (! m_pause) {
      get(NULL,NULL); // add to total
      m_pause = true;
   }
}

} } }


/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
