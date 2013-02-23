#ifndef BOOTES_LIB_FRAMEWORK_WIIMOTE_H
#define BOOTES_LIB_FRAMEWORK_WIIMOTE_H

#include "macros.h"
#include "InputEvent.h"

namespace bootes { namespace lib { namespace framework {

class Wiimote
{
public:
   Wiimote();
   virtual ~Wiimote();

   bool start();
   void stop();
   void poll(double currentTime, int elapsedTime); //値を読み取り、必要に応じて記憶や計算をする。

   bool isConnected() const;
   bool isMotionPlusEnabled() const;

   inline const WiimoteEvent* getEvent() const { return &_ev[_ei]; }
   inline bool isPressed(int b) const { return _ev[_ei].isPressed(b); }
   inline bool isHeld1sec(int b) const { return _ev[_ei].isHeld1sec(b); }
   inline bool isChanged(int b) const { return _ev[_ei^1].isPressed(b) ^ _ev[_ei].isPressed(b); }
   inline bool isChangePressed(int b) const { return isChanged(b) && isPressed(b); }
   inline bool isChangeReleased(int b) const { return isChanged(b) && !isPressed(b); }

   inline const WiimoteEvent::Accel& accel() const { return _ev[_ei].accel(); }
   inline const WiimoteEvent::Gyro&  gyro()  const { return _ev[_ei].gyro(); }
   inline const WiimoteEvent::Orien& orien() const { return _ev[_ei].orien(); }

public:
   inline bool isPressedA()     const { return isPressed(WiimoteEvent::BTN_A); }
   inline bool isPressedB()     const { return isPressed(WiimoteEvent::BTN_B); }
   inline bool isPressedPlus()  const { return isPressed(WiimoteEvent::BTN_PLUS); }
   inline bool isPressedHome()  const { return isPressed(WiimoteEvent::BTN_HOME); }
   inline bool isPressedMinus() const { return isPressed(WiimoteEvent::BTN_MINUS); }
   inline bool isPressed1()     const { return isPressed(WiimoteEvent::BTN_1); }
   inline bool isPressed2()     const { return isPressed(WiimoteEvent::BTN_2); }
   inline bool isPressedUp()    const { return isPressed(WiimoteEvent::BTN_UP); }
   inline bool isPressedDown()  const { return isPressed(WiimoteEvent::BTN_DOWN); }
   inline bool isPressedLeft()  const { return isPressed(WiimoteEvent::BTN_LEFT); }
   inline bool isPressedRight() const { return isPressed(WiimoteEvent::BTN_RIGHT); }

   inline bool isHeld1secA()     const { return isHeld1sec(WiimoteEvent::BTN_A); }
   inline bool isHeld1secB()     const { return isHeld1sec(WiimoteEvent::BTN_B); }
   inline bool isHeld1secPlus()  const { return isHeld1sec(WiimoteEvent::BTN_PLUS); }
   inline bool isHeld1secHome()  const { return isHeld1sec(WiimoteEvent::BTN_HOME); }
   inline bool isHeld1secMinus() const { return isHeld1sec(WiimoteEvent::BTN_MINUS); }
   inline bool isHeld1sec1()     const { return isHeld1sec(WiimoteEvent::BTN_1); }
   inline bool isHeld1sec2()     const { return isHeld1sec(WiimoteEvent::BTN_2); }
   inline bool isHeld1secUp()    const { return isHeld1sec(WiimoteEvent::BTN_UP); }
   inline bool isHeld1secDown()  const { return isHeld1sec(WiimoteEvent::BTN_DOWN); }
   inline bool isHeld1secLeft()  const { return isHeld1sec(WiimoteEvent::BTN_LEFT); }
   inline bool isHeld1secRight() const { return isHeld1sec(WiimoteEvent::BTN_RIGHT); }

protected:
   virtual bool calcData(WiimoteEvent* newev, const WiimoteEvent* oldev);

private:
   void* _proxy;
   WiimoteEvent _ev[2];
   int _ei;
};

} } }

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
