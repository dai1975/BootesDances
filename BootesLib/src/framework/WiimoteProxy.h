#ifndef BOOTES_LIB_SRC_FRAMEWROK_WIIMOTE_PROXY_H
#define BOOTES_LIB_SRC_FRAMEWROK_WIIMOTE_PROXY_H

#include "../include.h"
#include <bootes/lib/framework/Wiimote.h>
#include <wiimote-bootes.h> //WiiYourself!

namespace bootes { namespace lib { namespace framework {

class WiimoteProxy
{
public:
   WiimoteProxy();
   virtual ~WiimoteProxy();

   bool proxyStart();
   void proxyRun();
   void proxyStop();
   static DWORD WINAPI ProxyThreadFunc(LPVOID p);

   bool poll(double currentTime, int elapsedTime);
   inline const WiimoteEvent& getEvent() const { return _ev; }
   inline bool isConnected() const { return _remote.IsConnected(); }
   inline bool isMotionPlusEnabled() const { return _remote.MotionPlusEnabled(); }

private:
   wiimote _remote;
   WiimoteEvent _ev;
   bool _proxy_run;
   HANDLE _proxy_thread;
   DWORD  _proxy_thread_id;
   DWORD _time0;

   enum BUTTON_INDEX {
      BTN_IDX_A     = 0,
      BTN_IDX_B     = 1,
      BTN_IDX_PLUS  = 2,
      BTN_IDX_HOME  = 3,
      BTN_IDX_MINUS = 4,
      BTN_IDX_1     = 5,
      BTN_IDX_2     = 6,
      BTN_IDX_UP    = 7,
      BTN_IDX_DOWN  = 8,
      BTN_IDX_LEFT  = 9,
      BTN_IDX_RIGHT = 10,
      NUM_BUTTONS,
   };
   int _heldtime[NUM_BUTTONS];
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
