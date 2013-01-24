#ifndef BOOTESDANCES_MOTION_TEACH_H
#define BOOTESDANCES_MOTION_TEACH_H

#include <bootes/lib/framework/InputEvent.h>
#include <list>
#include <algorithm>

struct TeachSequence
{
public:
   TeachSequence();
   TeachSequence(const TeachSequence& r);
   TeachSequence& operator=(const TeachSequence& r);
   ~TeachSequence();

   void clear();
   void add(int t, const ::bootes::lib::framework::WiimoteEvent& wev);
   void commit(bool succeed);

public:
   struct Record {
      int time;
      ::bootes::lib::framework::WiimoteEvent wiimote;

      inline Record(): time(0) { }
      inline Record(int t, const ::bootes::lib::framework::WiimoteEvent& wev): time(0), wiimote(wev) { }
      inline Record(const Record& r) { operator=(r); }
      inline Record& operator=(const Record& r) {
         time = r.time;
         wiimote = r.wiimote;
         return *this;
      }
   };
   bool succeed;
   typedef std::list< Record > t_records;
   t_records records;

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
