#ifndef BOOTESDANCES_STAGE_STAGECLOCK_H
#define BOOTESDANCES_STAGE_STAGECLOCK_H

struct StageClock
{
public:
   __int64 clock; //100ns

   inline StageClock(): clock(0) { }
   inline StageClock(const StageClock& r) { operator=(r); }
   inline StageClock& operator=(const StageClock& r) { clock = r.clock; return *this; }
   inline ~StageClock() { }
};

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
