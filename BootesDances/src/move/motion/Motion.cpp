#include "Motion.h"

Motion::Motion()
 : _pMove(NULL)
 , _teach_state(TEACH_DEFAULT)
 , _test_state(TEST_DEFAULT)
{ }

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

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */