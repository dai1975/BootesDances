#ifndef BOOTESDANCES_MOTION_H
#define BOOTESDANCES_MOTION_H

#include <bootes/lib/framework/InputEvent.h>
#include "proto/Stage.pb.h"

class IMove;
class IMotion
{
public:
   inline virtual ~IMotion() { }
   virtual IMotion* clone() const = 0;

   virtual void setMove(IMove*) = 0;

   virtual void teachClear() = 0; //!< モーション定義をクリアする。

   virtual void teachBegin() = 0;
   virtual void teach(int t, const ::bootes::lib::framework::InputEvent* ev) = 0; //!< 新たな定義値を記録する
   virtual void teachCommit(bool succeed) = 0;
   virtual void teachRollback() = 0;

   virtual void testClear() = 0;
   virtual void testBegin() = 0;
   virtual void test(int t, const ::bootes::lib::framework::InputEvent* ev) = 0; //!< テストする
   virtual void testEnd(bool completed) = 0;

   enum TEACH_STATE { TEACH_DEFAULT, TEACH_TEACHING };
   virtual TEACH_STATE getTeachState() const = 0;
   
   enum TEST_STATE { TEST_DEFAULT, TEST_TESTING, TEST_SUCCEED, TEST_FAILED, };
   virtual TEST_STATE getTestState() const = 0;
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
