#include "Motion.h"
#include <algorithm>

Motion::Motion()
 : _pMove(NULL)
 , _teach_state(TEACH_DEFAULT)
 , _test_state(TEST_DEFAULT)
{ }

Motion::Motion(const Motion& r)
{
   std::copy(r._teaches.begin(), r._teaches.end(), std::back_inserter(_teaches));
}

Motion::~Motion()
{ }

void Motion::setMove(IMove* pMove)
{
   _pMove = pMove;
}

__int64 Motion::getDuration() const
{
   if (_pMove == NULL) { return 0; }
   return _pMove->getDuration();
}

IMotion::TEACH_STATE Motion::getTeachState() const
{
   return _teach_state;
}

IMotion::TEST_STATE  Motion::getTestState() const
{
   return _test_state;
}

void Motion::teachClear()
{
   _teaches.clear();
}

void Motion::teachBegin()
{
   _teaches.push_back( TeachSequence() );
}

void Motion::teach(int t, const ::bootes::lib::framework::InputEvent* ev)
{
   TeachSequence& teach = _teaches.back();
   if (ev->_type == ::bootes::lib::framework::InputEvent::T_WIIMOTE) {
      const ::bootes::lib::framework::WiimoteEvent* wev = static_cast< const ::bootes::lib::framework::WiimoteEvent* >(ev);
      teach.records.push_back( TeachSequence::Record(t, *wev) );
   }
}

void Motion::teachRollback()
{
   _teaches.pop_back();
}

void Motion::teachCommit(bool succeed)
{
   TeachSequence& teach = _teaches.back();
   teach.succeed = succeed;
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
