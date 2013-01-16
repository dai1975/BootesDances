#include "../include.h"
#include <bootes/lib/framework/Wiimote.h>
#include "WiimoteProxy.h"

namespace bootes { namespace lib { namespace framework {

Wiimote::Wiimote()
{
   _ei = 0;
   _proxy = new WiimoteProxy();
}

Wiimote::~Wiimote()
{
   if (_proxy) {
      stop();
      WiimoteProxy* p = static_cast< WiimoteProxy* >(_proxy);
      _proxy = NULL;
      delete p;
   }
};

bool Wiimote::start()
{
   if (_proxy) {
      return static_cast< WiimoteProxy* >(_proxy)->proxyStart();
   } else {
      return false;
   }
}

void Wiimote::stop()
{
   if (_proxy) {
      static_cast< WiimoteProxy* >(_proxy)->proxyStop();
   }
}

bool Wiimote::isConnected() const
{
   if (_proxy) {
      return static_cast< WiimoteProxy* >(_proxy)->isConnected();
   } else {
      return false;
   }
}

bool Wiimote::isMotionPlusEnabled() const
{
   if (_proxy) {
      return static_cast< WiimoteProxy* >(_proxy)->isMotionPlusEnabled();
   } else {
      return false;
   }
}

bool Wiimote::calcData(WiimoteEvent* newev, const WiimoteEvent* oldev)
{
   return true;
}

void Wiimote::poll(double currentTime, int elapsedTime)
{
   if (! _proxy) { return; }
   WiimoteProxy* proxy = static_cast< WiimoteProxy* >(_proxy);

   if (! proxy->isMotionPlusEnabled()) { return; }
   if (! proxy->poll(currentTime, elapsedTime)) { return; }
   if (! proxy->isMotionPlusEnabled()) { return; }

   int ei1 = _ei ^ 1;
   _ev[ei1] = proxy->getEvent();
   _ev[ei1] = proxy->getEvent();
   if (! calcData(&_ev[ei1], &_ev[_ei])) { return; }
   _ei ^= 1;
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
