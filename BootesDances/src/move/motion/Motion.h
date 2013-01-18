#ifndef BOOTESDANCES_MOTION_MOTION_H
#define BOOTESDANCES_MOTION_MOTION_H

#include <bootes/lib/framework/InputEvent.h>
#include <bootes/dances/IMotion.h>
#include <bootes/dances/IMove.h>

class Motion: public IMotion
{
public:
   Motion();
   virtual ~Motion();
   virtual void setMove(IMove* pMove);

   virtual __int64 getDuration() const;
   
   virtual TEACH_STATE getTeachState() const;
   virtual TEST_STATE getTestState() const;

protected:
   IMove* _pMove;
   TEACH_STATE _teach_state;
   TEST_STATE _test_state;
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
