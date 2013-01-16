#ifndef BOOTESDANCES_MOTION_MOTION_H
#define BOOTESDANCES_MOTION_MOTION_H

#include <bootes/dances/proto/Motion.pb.h>
#include <bootes/lib/framework/InputEvent.h>

class Motion
{
public:
   static Motion* Realize(const ::pb::Motion* idea);
   static ::pb::Motion* Idealize(const Motion* rea);

   virtual bool idealize(::pb::Motion*) const = 0;
   virtual bool realize(const ::pb::Motion*) = 0;

   enum DEVICE {
      WIIMOTE,
      KINECT,
      NUM_DEVICES,
   };
   enum DETECTOR {
      WIIMOTE_SIMPLE,
   };

protected:
   explicit Motion(DEVICE, DETECTOR);

public:
   virtual ~Motion();
   virtual Motion* clone() const = 0;
   inline int getDeviceType() const { return _device_type; }
   inline int getDetectorType() const { return _detector_type; }

   void setDuration(int dur); //!< モーションの持続時間
   int getDuration() const;
   
   virtual void teachClear() = 0; //!< モーション定義をクリアする。

   virtual void teachBegin() = 0;
   virtual void teach(const ::bootes::lib::framework::InputEvent* ev, int t) = 0; //!< 新たな定義値を記録する
   virtual void teachCommit(bool succeed) = 0;
   virtual void teachRollback() = 0;

   virtual void testClear() = 0;
   virtual void testBegin() = 0;
   virtual void test(const ::bootes::lib::framework::InputEvent* ev, int t) = 0; //!< テストする
   virtual void testEnd(bool completed) = 0;

   enum TEACH_STATE { TEACH_DEFAULT, TEACH_TEACHING };
   inline TEACH_STATE getTeachState() const { return _teach_state; }

   enum TEST_STATE { TEST_DEFAULT, TEST_TESTING, TEST_SUCCEED, TEST_FAILED, };
   inline TEST_STATE getTestState() const { return _test_state; }

protected:
   int _device_type;
   int _detector_type;
   int _duration;
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
