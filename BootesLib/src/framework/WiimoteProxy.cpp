#include "WiimoteProxy.h"
#include <mmsystem.h>

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
   : _buffer(200), _buffer_swap(200)
{
   _proxy_thread = INVALID_HANDLE_VALUE;
   _proxy_thread_id = 0;
   _remote.CallbackTriggerFlags = (state_change_flags)
      (CONNECTED | EXTENSION_CHANGED | MOTIONPLUS_CHANGED);
   _remote.ChangedCallback = WiimoteHandler;
   _hMutex = CreateMutex(NULL, FALSE, NULL);
   _last_event_timestamp = 0;
}

WiimoteProxy::~WiimoteProxy()
{
   CloseHandle(_hMutex);
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
         if (! _remote.Connect(wiimote::FIRST_AVAILABLE)) {
            Sleep(1000);
         }

      } else if (! _remote.MotionPlusEnabled()) {
         Sleep(1000);
         
      } else {
         if (_buffer.full()) {
            Sleep(1);
         } else {
            WiimoteEvent ev;
            if (! poll(&ev)) {
               Sleep(1);
            } else if (_last_event_timestamp == ev._event_timestamp) {
               Sleep(1);
            } else {
               WaitForSingleObject(_hMutex, INFINITE);
               _buffer.push_back(ev);
               ReleaseMutex(_hMutex);
               _last_event_timestamp = ev._event_timestamp;
            }
         }
      }
   }
   if (_remote.IsConnected()) {
      _remote.Disconnect();
   }
   _proxy_thread = INVALID_HANDLE_VALUE;
}

size_t WiimoteProxy::consumeEvents(std::list< WiimoteEvent >* pOut)
{
   if (_buffer.empty()) { return 0; }

   size_t r = 0;
   WiimoteEvent ev;
   WaitForSingleObject(_hMutex, INFINITE);
   {
      _buffer_swap.clear();
      _buffer.swap(_buffer_swap);
      while (! _buffer_swap.empty()) {
         _buffer_swap.pop_front(ev);
         pOut->push_back(ev);
         ++r;
      }
   }
   ReleaseMutex(_hMutex);
   return r;
}

bool WiimoteProxy::poll(WiimoteEvent* pEv)
{
   pEv->clear();
   pEv->_bConnect = _remote.IsConnected();
   pEv->_bMplus   = _remote.MotionPlusEnabled();

   if (! pEv->_bMplus) { return false; }

   if (_remote.RefreshState() == 0) { return false; }
   DWORD t = timeGetTime();
   pEv->_event_timestamp = t;

#define IMPL_BUTTON(YOUR_NAME, MY_NAME)\
   if (_remote.Button. YOUR_NAME ()) {                       \
      pEv->_pressed |= WiimoteEvent::BTN_ ## MY_NAME;         \
      if (_heldstarttime[ BTN_IDX_ ## MY_NAME ] == 0) {\
         _heldstarttime[ BTN_IDX_ ## MY_NAME ] = t;\
      } else {\
         int t0 = _heldstarttime[ BTN_IDX_ ## MY_NAME ];                \
         int heldtime = (t0 < t)? (t-t0): (0xffffffffUL - (t0 - t));    \
         if (1000 <= heldtime) {                                        \
            pEv->_held1 |= WiimoteEvent::BTN_ ## MY_NAME;                \
         }\
      }\
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

   pEv->_accel.t    = _remote.Acceleration.Time;
   pEv->_accel.x    = _remote.Acceleration.X;
   pEv->_accel.y    = _remote.Acceleration.Y;
   pEv->_accel.z    = _remote.Acceleration.Z;
   pEv->_gyro.t     = _remote.MotionPlus.Time;
   pEv->_gyro.yaw   = _remote.MotionPlus.Speed.Yaw;
   pEv->_gyro.pitch = _remote.MotionPlus.Speed.Pitch;
   pEv->_gyro.roll  = _remote.MotionPlus.Speed.Roll;

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
