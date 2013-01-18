#ifndef BOOTESDANCES_MOTION_WIIMOTE_MOTION_SIMPLE_H
#define BOOTESDANCES_MOTION_WIIMOTE_MOTION_SIMPLE_H

#include "Motion.h"
#include <list>
#include <vector>
#include <string>

class MotionWiimoteSimple: public Motion
{
public:
   MotionWiimoteSimple();
   virtual ~MotionWiimoteSimple();
   virtual IMotion* clone() const;

   static const std::string TYPE;
   virtual bool idealize(::pb::Motion* pOut) const;
   virtual bool realize(const ::pb::Motion* pIn);

   virtual void teachClear(); //!< モーション定義をクリアする。

   virtual void teachBegin();
   virtual void teach(const ::bootes::lib::framework::InputEvent* ev, int t); //!< 新たな定義値を記録する
   virtual void teachCommit(bool succeed);
   virtual void teachRollback();

   virtual void testClear();
   virtual void testBegin();
   virtual void test(const ::bootes::lib::framework::InputEvent* ev, int t); //!< テストする
   virtual void testEnd(bool completed);

private:
   struct Entry {
      int t;
      ::bootes::lib::framework::WiimoteEvent ev;
      inline Entry(): t(0) { }
      inline Entry(int t_, const ::bootes::lib::framework::WiimoteEvent& ev_) { t = t_; ev = ev_; }
      inline Entry(const Entry& r) { operator=(r); }
      inline const Entry& operator=(const Entry& r) { t = r.t; ev = r.ev; return *this; }
   };
   typedef std::list< Entry > t_sequence;
   typedef std::list< t_sequence > t_data;
   t_data _succeed_data;
   t_data _failed_data;
   t_sequence _tmp_sequence;
   int _stept;

   t_sequence _test_seq_min, _test_seq_max;
   Entry  _test_lastgiven;
   t_sequence::iterator _test_min_nexti,_test_max_nexti;
   int _nFails;
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
