#include "WiimoteProxy.h"

namespace bootes { namespace lib { namespace framework {

namespace {

void WiimoteHandler(wiimote &remote, state_change_flags changed, const wiimote_state &new_state)
{
   if (changed & CONNECTED) {
      if (new_state.ExtensionType != wiimote::BALANCE_BOARD) {
         if (new_state.bExtension)
            remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT); // no IR dots
         else
            remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);	// IR dots
      }
   }

   // a MotionPlus was detected
   if (changed & MOTIONPLUS_DETECTED) {
      if (remote.ExtensionType == wiimote_state::NONE) {
         bool res = remote.EnableMotionPlus();
         //_ASSERT(res);
      }

   } else if (changed & MOTIONPLUS_EXTENSION_CONNECTED) {
      if (remote.MotionPlusEnabled())
         remote.DisableMotionPlus();

   } else if (changed & MOTIONPLUS_EXTENSION_DISCONNECTED) {
      if (remote.MotionPlusConnected())
         remote.EnableMotionPlus();

   } else if (changed & EXTENSION_CONNECTED) {
      if(!remote.IsBalanceBoard())
         remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR_EXT);

   } else if (changed & EXTENSION_DISCONNECTED) {
      remote.SetReportType(wiimote::IN_BUTTONS_ACCEL_IR);
   }
}

}

WiimoteProxy::WiimoteProxy()
{
   _proxy_thread = INVALID_HANDLE_VALUE;
   _proxy_thread_id = 0;
   _remote.CallbackTriggerFlags = (state_change_flags)
      (CONNECTED | EXTENSION_CHANGED | MOTIONPLUS_CHANGED);
   _remote.ChangedCallback = WiimoteHandler;
}

WiimoteProxy::~WiimoteProxy()
{
   if (_proxy_thread != INVALID_HANDLE_VALUE) {
      HANDLE h = _proxy_thread;
      proxyStop();
      if (h != INVALID_HANDLE_VALUE) {
         WaitForSingleObject(h, INFINITE);
      }
   }
}

bool WiimoteProxy::proxyStart()
{
   if (_proxy_thread != INVALID_HANDLE_VALUE) { return true; }

   _proxy_thread = CreateThread(NULL, 0, &WiimoteProxy::ProxyThreadFunc, this, 0, &_proxy_thread_id);
   return (_proxy_thread != INVALID_HANDLE_VALUE);
}

void WiimoteProxy::proxyStop()
{
   _proxy_run = false;
}

DWORD WINAPI WiimoteProxy::ProxyThreadFunc(LPVOID p)
{
   WiimoteProxy* obj = static_cast< WiimoteProxy* >(p);
   obj->proxyRun();
   return 0;
}

void WiimoteProxy::proxyRun()
{
   // this thread treat long term work such as connect.
   _proxy_run = true;
   while (_proxy_run) {
      if (! _remote.IsConnected()) {
         memset(_heldtime, 0, sizeof(_heldtime));
         if (! _remote.Connect(wiimote::FIRST_AVAILABLE)) {
            Sleep(1000);
         }

      } else if (! _remote.MotionPlusEnabled()) {
         Sleep(1000);
         
      } else {
         Sleep(1000);
      }
   }
   if (_remote.IsConnected()) {
      _remote.Disconnect();
   }
   _proxy_thread = INVALID_HANDLE_VALUE;
}

bool WiimoteProxy::poll(double currentTime, int elapsedTime)
{
   WiimoteEvent tmp;
   tmp.clear();
   tmp._bConnect = _remote.IsConnected();
   tmp._bMplus   = _remote.MotionPlusEnabled();

   if (! tmp._bMplus) { return false; }

   if (_remote.RefreshState() == 0) { return false; }

#define IMPL_BUTTON(YOUR_NAME, MY_NAME)\
   if (_remote.Button. YOUR_NAME ()) {                       \
      tmp._pressed |= WiimoteEvent::BTN_ ## MY_NAME;         \
      _heldtime[ BTN_IDX_ ## MY_NAME ] += elapsedTime;      \
      if (1000 <= _heldtime[ BTN_IDX_ ## MY_NAME ]) {\
         tmp._held1 |= WiimoteEvent::BTN_ ## MY_NAME; \
      }\
   } else {\
      _heldtime[ BTN_IDX_ ## MY_NAME ] = 0;\
   }
   IMPL_BUTTON(A, A);
   IMPL_BUTTON(B, B);
   IMPL_BUTTON(Plus, PLUS);
   IMPL_BUTTON(Home, HOME);
   IMPL_BUTTON(Minus, MINUS);
   IMPL_BUTTON(One, 1);
   IMPL_BUTTON(Two, 2);
   IMPL_BUTTON(Up, UP);
   IMPL_BUTTON(Down, DOWN);
   IMPL_BUTTON(Left, LEFT);
   IMPL_BUTTON(Right, RIGHT);
#undef IMPL_BUTTON

   tmp._accel.t    = _remote.Acceleration.Time;
   tmp._accel.x    = _remote.Acceleration.X;
   tmp._accel.y    = _remote.Acceleration.Y;
   tmp._accel.z    = _remote.Acceleration.Z;
   tmp._gyro.t     = _remote.MotionPlus.Time;
   tmp._gyro.yaw   = _remote.MotionPlus.Speed.Yaw;
   tmp._gyro.pitch = _remote.MotionPlus.Speed.Pitch;
   tmp._gyro.roll  = _remote.MotionPlus.Speed.Roll;

   _ev = tmp;
   return true;
}

} } }

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
