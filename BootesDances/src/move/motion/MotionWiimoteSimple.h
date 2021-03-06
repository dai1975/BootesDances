#ifndef BOOTESDANCES_MOTION_WIIMOTE_MOTION_SIMPLE_H
#define BOOTESDANCES_MOTION_WIIMOTE_MOTION_SIMPLE_H

#include "Motion.h"
#include <list>
#include <vector>
#include <string>
#include "MotionWiimoteSimple.pb.h"

class MotionWiimoteSimple: public Motion
{
protected:
   MotionWiimoteSimple(const MotionWiimoteSimple&);
public:
   MotionWiimoteSimple();
   virtual ~MotionWiimoteSimple();
   virtual IMotion* clone() const { return new MotionWiimoteSimple(*this); }

   virtual bool realize(const ::pb::MotionWiimoteSimple& in);
   virtual bool idealize(::pb::MotionWiimoteSimple* pOut) const;

   virtual void teachClear(); //!< モーション定義をクリアする。
   virtual void teachBegin();
   virtual void teach(int t, const ::bootes::lib::framework::InputEvent* ev); //!< 新たな定義値を記録する
   virtual void teachCommit(bool succeed);
   virtual void teachRollback();

   virtual void testClear();
   virtual void testBegin();
   virtual void test(int t, const ::bootes::lib::framework::InputEvent* ev); //!< テストする
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
   t_sequence _tmp_sequence;
   int _stept;
   std::string _uuid;
   __int64 _time0, _time1;

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
