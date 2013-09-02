#ifndef BOOTES_LIB_FRAMEWORK_INPUTEVENT_H
#define BOOTES_LIB_FRAMEWORK_INPUTEVENT_H

#include "macros.h"
#include "../win.h"
#include <string>

namespace bootes { namespace lib { namespace framework {

struct InputEvent
{
public:
   enum Type {
      T_WNDMSG,
      T_WIIMOTE,
      T_KINECT,
   };
   Type _type;
protected:
   inline InputEvent(Type t): _type(t) { }
};

struct WndmsgEvent: public InputEvent
{
public:
   inline WndmsgEvent(): InputEvent(T_WNDMSG) { }

   HWND _hWnd;
   UINT _message;
   WPARAM _wParam;
   LPARAM _lParam;
};

struct WiimoteEvent: public InputEvent
{
public:
   inline WiimoteEvent(): InputEvent(T_WIIMOTE) { clear(); }
   inline const WiimoteEvent& operator=(const WiimoteEvent& r);
   inline void clear();

   enum BUTTON {
      BTN_A     = 1 << 0,
      BTN_B     = 1 << 1,
      BTN_PLUS  = 1 << 2,
      BTN_HOME  = 1 << 3,
      BTN_MINUS = 1 << 4,
      BTN_1     = 1 << 5,
      BTN_2     = 1 << 6,
      BTN_UP    = 1 << 7,
      BTN_DOWN  = 1 << 8,
      BTN_LEFT  = 1 << 9,
      BTN_RIGHT = 1 << 10,
   };
   struct Accel {
      DWORD t;      // t=timeGetTime[msec]

      // -3.0f..3.0f [g]. 1.0f means 1g. For example, no force results near (0, 0, 1)
      float x; // plus direction: right
      float y; // plus direction: forward
      float z; // plus direction: down
   };
   struct Gyro {
      DWORD t;      // t=timeGetTime[msec]
      // [deg/sec]. 0.0f means paused;
      float yaw;   // plus direction: head-right
      float pitch; // plus direction: head-up
      float roll;  // plus direction: body-right
   }; 
   struct Orien { //orientation
      float x,y,z; // z-axis
      float yaw,pitch,roll;  // degree
   };

   inline bool   isConnect() const { return _bConnect; }
   inline bool   isMotionPlusEnabled() const { return _bMplus; }
   inline bool   isPressed(int b) const { return ((_pressed & b) == b); }
   inline bool   isHeld1sec(int b) const { return ((_held1 & b) == b); }
   inline const Accel& accel() const { return _accel; }
   inline const Gyro&  gyro()  const { return _gyro; }
   inline const Orien& orien()  const { return _orien; }

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

public:
   __int64 _event_timestamp;
   bool _bConnect, _bMplus;
   int _pressed, _held1;
   Accel _accel; //[g]. [-3.0, 3.0]
   Gyro _gyro;   //[deg/sec]
   Orien _orien;
};

inline void WiimoteEvent::clear()
{
   _event_timestamp = 0;
   _bConnect   = false;
   _bMplus     = false;
   _pressed    = 0;
   _held1      = 0;
   _accel.t    = 0;
   _accel.x    = 0;
   _accel.y    = 0;
   _accel.z    = 0;
   _gyro.t     = 0;
   _gyro.yaw   = 0;
   _gyro.pitch = 0;
   _gyro.roll  = 0;
   _orien.x    = 0;
   _orien.y    = 0;
   _orien.z    = 0;
   _orien.yaw  = 0;
   _orien.pitch= 0;
   _orien.roll = 0;
}
inline const WiimoteEvent& WiimoteEvent::operator=(const WiimoteEvent& r)
{
   _event_timestamp = r._event_timestamp;
   _bConnect   = r._bConnect;
   _bMplus     = r._bMplus;
   _pressed    = r._pressed;
   _held1      = r._held1;
   _accel.t    = r._accel.t;
   _accel.x    = r._accel.x;
   _accel.y    = r._accel.y;
   _accel.z    = r._accel.z;
   _gyro.t     = r._gyro.t;
   _gyro.yaw   = r._gyro.yaw;
   _gyro.pitch = r._gyro.pitch;
   _gyro.roll  = r._gyro.roll;
   _orien.x    = r._orien.x;
   _orien.y    = r._orien.y;
   _orien.z    = r._orien.z;
   _orien.yaw  = r._orien.yaw;
   _orien.pitch= r._orien.pitch;
   _orien.roll = r._orien.roll;
   return *this;
}

} } }

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
