#ifndef BOOTESDANCES_MOTION_MOTION_H
#define BOOTESDANCES_MOTION_MOTION_H

#include <bootes/lib/framework/InputEvent.h>
#include <bootes/dances/IMotion.h>
#include <bootes/dances/IMove.h>
#include "Teach.h"
#include <list>

class Motion: public IMotion
{
protected:
   Motion(const Motion&);
public:
   Motion();
   virtual ~Motion();
   virtual void setMove(IMove* pMove);

   virtual __int64 getDuration() const;
   
   virtual TEACH_STATE getTeachState() const;
   virtual TEST_STATE getTestState() const;

   virtual void teachClear(); //!< モーション定義をクリアする。
   virtual void teachBegin();
   virtual void teach(int t, const ::bootes::lib::framework::InputEvent* ev); //!< 新たな定義値を記録する
   virtual void teachCommit(bool succeed);
   virtual void teachRollback();

protected:
   IMove* _pMove;
   TEACH_STATE _teach_state;
   TEST_STATE _test_state;

private:
   typedef std::list< TeachSequence > t_teaches;
   t_teaches _teaches;
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
