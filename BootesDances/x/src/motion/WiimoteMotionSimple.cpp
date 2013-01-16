#include "../include.h"
#include "WiimoteMotionSimple.h"
#include <algorithm>

using ::bootes::lib::framework::InputEvent;
using ::bootes::lib::framework::WiimoteEvent;

WiimoteMotionSimple::WiimoteMotionSimple()
   : Motion(WIIMOTE, WIIMOTE_SIMPLE)
{
   _stept = 100*10000; //100ms
   _test_state = TEST_DEFAULT;
   _teach_state = TEACH_DEFAULT;
}

WiimoteMotionSimple::~WiimoteMotionSimple()
{
}

Motion* WiimoteMotionSimple::clone() const
{
   WiimoteMotionSimple* r = new WiimoteMotionSimple();
   r->_duration = _duration;
   r->_stept    = _stept;
   for (t_data::const_iterator i = _succeed_data.begin(); i != _succeed_data.end(); ++i) {
      r->_succeed_data.push_back( t_sequence() );
      for (t_sequence::const_iterator j = i->begin(); j != i->end(); ++j) {
         r->_succeed_data.back().push_back( *j );
      }
   }
   for (t_data::const_iterator i = _failed_data.begin(); i != _failed_data.end(); ++i) {
      r->_failed_data.push_back( t_sequence() );
      for (t_sequence::const_iterator j = i->begin(); j != i->end(); ++j) {
         r->_failed_data.back().push_back( *j );
      }
   }
   return r;
}

bool WiimoteMotionSimple::idealize(::pb::Motion* idea0) const
{
   idea0->set_duration(_duration);
   {
      ::pb::Motion::WiimoteSimple* idea = idea0->mutable_wiimote_simple();
      idea->set_stept(_stept);
      for (t_sequence::const_iterator i = _test_seq_min.begin(); i != _test_seq_min.end(); ++i) {
         const Entry& r = *i;
         ::pb::Motion::WiimoteSimple::Entry* record = idea->add_minseq();
         record->set_t(r.t);
         record->mutable_accel()->set_x(    r.ev._accel.x );
         record->mutable_accel()->set_y(    r.ev._accel.y );
         record->mutable_accel()->set_z(    r.ev._accel.z );
         record->mutable_gyro()->set_yaw(   r.ev._gyro.yaw );
         record->mutable_gyro()->set_pitch( r.ev._gyro.pitch );
         record->mutable_gyro()->set_roll(  r.ev._gyro.roll );
         record->mutable_orien()->set_yaw(   r.ev._orien.yaw );
         record->mutable_orien()->set_pitch( r.ev._orien.pitch );
         record->mutable_orien()->set_roll(  r.ev._orien.roll );
      }
      for (t_sequence::const_iterator i = _test_seq_max.begin(); i != _test_seq_max.end(); ++i) {
         const Entry& r = *i;
         ::pb::Motion::WiimoteSimple::Entry* record = idea->add_maxseq();
         record->set_t(r.t);
         record->mutable_accel()->set_x(    r.ev._accel.x );
         record->mutable_accel()->set_y(    r.ev._accel.y );
         record->mutable_accel()->set_z(    r.ev._accel.z );
         record->mutable_gyro()->set_yaw(   r.ev._gyro.yaw );
         record->mutable_gyro()->set_pitch( r.ev._gyro.pitch );
         record->mutable_gyro()->set_roll(  r.ev._gyro.roll );
         record->mutable_orien()->set_yaw(   r.ev._orien.yaw );
         record->mutable_orien()->set_pitch( r.ev._orien.pitch );
         record->mutable_orien()->set_roll(  r.ev._orien.roll );
      }
   }
   return true;
}

bool WiimoteMotionSimple::realize(const ::pb::Motion* idea0)
{
   teachClear();
   const ::pb::Motion::WiimoteSimple* idea = &idea0->wiimote_simple();
   _stept = idea->stept();
   for (size_t i=0; i<idea->minseq_size(); ++i) {
      const ::pb::Motion::WiimoteSimple::Entry& r0 = idea->minseq(i);
      _test_seq_min.push_back( Entry() );
      Entry& r = _test_seq_min.back();
      r.t = r0.t();
      r.ev._accel.x    = r0.accel().x();
      r.ev._accel.y    = r0.accel().y();
      r.ev._accel.z    = r0.accel().z();
      r.ev._gyro.yaw   = r0.gyro().yaw();
      r.ev._gyro.roll  = r0.gyro().roll();
      r.ev._gyro.pitch = r0.gyro().pitch();
      r.ev._orien.yaw   = r0.orien().yaw();
      r.ev._orien.roll  = r0.orien().roll();
      r.ev._orien.pitch = r0.orien().pitch();
   }
   for (size_t i=0; i<idea->maxseq_size(); ++i) {
      const ::pb::Motion::WiimoteSimple::Entry& r0 = idea->maxseq(i);
      _test_seq_max.push_back( Entry() );
      Entry& r = _test_seq_max.back();
      r.t = r0.t();
      r.ev._accel.x    = r0.accel().x();
      r.ev._accel.y    = r0.accel().y();
      r.ev._accel.z    = r0.accel().z();
      r.ev._gyro.yaw   = r0.gyro().yaw();
      r.ev._gyro.roll  = r0.gyro().roll();
      r.ev._gyro.pitch = r0.gyro().pitch();
      r.ev._orien.yaw   = r0.orien().yaw();
      r.ev._orien.roll  = r0.orien().roll();
      r.ev._orien.pitch = r0.orien().pitch();
   }
   return true;
}

void WiimoteMotionSimple::teachClear()
{
   _succeed_data.clear();
   _failed_data.clear();
   _test_seq_min.clear();
   _test_seq_max.clear();
}
void WiimoteMotionSimple::testClear()
{
   _test_state = TEST_DEFAULT;
}

namespace {

WiimoteEvent CalcPoint(int t0, const WiimoteEvent& ev0, int t1, const WiimoteEvent& ev1, int t)
{
   WiimoteEvent ev;
   float f = ((float)(t - t0)) / (t1 - t0);
   // 回転は線形補完できないが
   ev._accel.x     = ev0._accel.x     + f * (ev1._accel.x     - ev0._accel.x);
   ev._accel.y     = ev0._accel.y     + f * (ev1._accel.y     - ev0._accel.y);
   ev._accel.z     = ev0._accel.z     + f * (ev1._accel.z     - ev0._accel.z);
   ev._gyro.yaw    = ev0._gyro.yaw    + f * (ev1._gyro.yaw    - ev0._gyro.yaw);
   ev._gyro.pitch  = ev0._gyro.pitch  + f * (ev1._gyro.pitch  - ev0._gyro.pitch);
   ev._gyro.roll   = ev0._gyro.roll   + f * (ev1._gyro.roll   - ev0._gyro.roll);
   ev._orien.x     = ev0._orien.x     + f * (ev1._orien.x     - ev0._orien.x);
   ev._orien.y     = ev0._orien.y     + f * (ev1._orien.y     - ev0._orien.y);
   ev._orien.z     = ev0._orien.z     + f * (ev1._orien.z     - ev0._orien.z);
   ev._orien.yaw   = ev0._orien.yaw   + f * (ev1._orien.yaw   - ev0._orien.yaw);
   ev._orien.pitch = ev0._orien.pitch + f * (ev1._orien.pitch - ev0._orien.pitch);
   ev._orien.roll  = ev0._orien.roll  + f * (ev1._orien.roll  - ev0._orien.roll);
   return ev;
}

}

void WiimoteMotionSimple::teachBegin()
{
   _tmp_sequence.clear();
   _teach_state = TEACH_TEACHING;
}

void WiimoteMotionSimple::teach(const InputEvent* ev, int t)
{
   if (ev->_type != InputEvent::T_WIIMOTE) { return; }
   if (t < -_stept || _duration + _stept < t) { return; }

   const WiimoteEvent* wev = static_cast< const WiimoteEvent*>(ev);
   _tmp_sequence.push_back( Entry(t, *wev) );
}

void WiimoteMotionSimple::teachRollback()
{
   _tmp_sequence.clear();
   _teach_state = TEACH_DEFAULT;
}

void WiimoteMotionSimple::dump(const TCHAR* name, t_data& data)
{
   std::basic_string< TCHAR > path;
   path += g_pGame->getUserDir();
   path += _T('\\');
   path += name;

   char buf[1024];
   DWORD w;
   HANDLE fh = CreateFile(path.c_str(), GENERIC_WRITE, 0, NULL,
                          CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
   for (t_data::iterator i = data.begin(); i != data.end(); ++i) {
      for (t_sequence::iterator j = i->begin(); j != i->end(); ++j) {
         Entry& e = *j;
         int s = sprintf_s(buf, 1024, "%7d,%f,%f,%f\n", e.t, e.ev._accel.x, e.ev._accel.y, e.ev._accel.z);
         WriteFile(fh, buf, s, &w, NULL);
      }
      WriteFile(fh, "\n", 1, &w, NULL);
   }
   CloseHandle(fh);
}

void WiimoteMotionSimple::teachCommit(bool succeed)
{
   _teach_state = TEACH_DEFAULT;

   if (_tmp_sequence.empty()) { return; }
   if (succeed) {
      _succeed_data.push_back(_tmp_sequence);
      dump(_T("succeed.txt"), _succeed_data);
   } else {
      _failed_data.push_back(_tmp_sequence);
      dump(_T("failed.txt"), _failed_data);
   }

   if (! succeed) { return; }


   // _records を _stept 単位で正規化する
   t_sequence out;
   out.clear();

   t_sequence::iterator i = _tmp_sequence.begin();
   Entry* r0 = &(*i);
   int t1 = _stept; //テストは t=_stept から行う
   for (++i; i != _tmp_sequence.end(); ++i) {
      Entry* r1 = &(*i);
      if (r0->t != r1->t && t1 <= r1->t) {
         Entry r;
         r.t = t1;
         r.ev = CalcPoint(r0->t, r0->ev, r1->t, r1->ev, t1);
         out.push_back(r);
         
         if (_duration <= t1) { break; }
         t1 += _stept;
         if (_duration <= t1) { t1 = _duration; }
      }
      r0 = r1;
   }

   if (_test_seq_min.empty()) {
      for (t_sequence::iterator i = out.begin(); i != out.end(); ++i) {
         _test_seq_min.push_back(*i);
      }
      _test_seq_max.swap(out);
      return;
   }

   i  = out.begin();
   t_sequence::iterator i0 = _test_seq_min.begin();
   t_sequence::iterator i1 = _test_seq_max.begin();
   while (i != out.end()) {
      //if ((*i).ev._accel.x < (*i0).ev._accel.x) { (*i0).ev._accel.x = (*i).ev._accel.x; }
#define SETMIN(PROP)  if ((*i).ev.PROP < (*i0).ev.PROP) { (*i0).ev.PROP = (*i).ev.PROP; }
#define SETMAX(PROP)  if ((*i).ev.PROP > (*i1).ev.PROP) { (*i1).ev.PROP = (*i).ev.PROP; }
#define SETMINMAX(PROP) SETMIN(PROP) SETMAX(PROP)
      SETMINMAX(_accel.x);
      SETMINMAX(_accel.y);
      SETMINMAX(_accel.z);
      SETMINMAX(_gyro.yaw);
      SETMINMAX(_gyro.pitch);
      SETMINMAX(_gyro.roll);
      SETMINMAX(_orien.x);
      SETMINMAX(_orien.y);
      SETMINMAX(_orien.z);
      SETMINMAX(_orien.yaw);
      SETMINMAX(_orien.pitch);
      SETMINMAX(_orien.roll);

      ++i; ++i0; ++i1;
   }
}

void WiimoteMotionSimple::testBegin()
{
   _test_state = TEST_TESTING;
   _test_lastgiven.t = -_stept;
   _test_lastgiven.ev.clear();
   _test_min_nexti = _test_seq_min.begin();
   _test_max_nexti = _test_seq_max.begin();
   _nFails = 0;
}

void WiimoteMotionSimple::testEnd(bool completed)
{
   if (_test_state != TEST_TESTING) { return; }

   if (! completed) {
      _test_min_nexti = _test_seq_min.end();
      _test_max_nexti = _test_seq_max.end();
      _test_state = TEST_FAILED;
   }
   if (_test_min_nexti != _test_seq_min.end()) {
      _test_state = TEST_FAILED;
   } else {
      _test_state = TEST_SUCCEED;
   }
}

void WiimoteMotionSimple::test(const InputEvent* given_ev0, int t)
{
   if (given_ev0->_type != InputEvent::T_WIIMOTE) { return; }
   if (_test_state != TEST_TESTING) { return; }
   if (_test_min_nexti == _test_seq_min.end()) {
      return;
   }

   const WiimoteEvent* given_ev = static_cast< const WiimoteEvent* >(given_ev0);
   Entry& exp_min = *_test_min_nexti;
   Entry& exp_max = *_test_max_nexti;
   if (t < exp_min.t) {
      _test_lastgiven.t = t;
      _test_lastgiven.ev = *given_ev;
      return;
   }

   WiimoteEvent given = CalcPoint(_test_lastgiven.t, _test_lastgiven.ev, t, *given_ev, exp_min.t);
   bool r = true;
#define TEST(MEM, DIFF)\
   if (r && (given.MEM < exp_min.ev.MEM || exp_max.ev.MEM < given.MEM)) { r = false; }

   float diff;

   diff = 0.5f;
   TEST(_accel.x, diff);
   TEST(_accel.y, diff);
   TEST(_accel.z, diff);
/*
   diff = 1000.0f;
   TEST(_gyro.yaw, diff);
   TEST(_gyro.pitch, diff);
   TEST(_gyro.roll, diff);

   diff = 50.0f;
   TEST(_orien.yaw, diff);
   TEST(_orien.pitch, diff);
   TEST(_orien.roll, diff);
*/
#undef TEST
   if (r) {
      _nFails = 0;
   } else {
      ++_nFails;
      if (2 < _nFails || _nFails == _test_seq_min.size()) {
         _test_state = TEST_FAILED;
      }
   }

   _test_lastgiven.t  = t;
   _test_lastgiven.ev = *given_ev;
   ++_test_min_nexti;
   ++_test_max_nexti;
}


/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
