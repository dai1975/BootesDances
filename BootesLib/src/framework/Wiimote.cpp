#include "../include.h"
#include <bootes/lib/framework/Wiimote.h>
#include "WiimoteProxy.h"

namespace bootes { namespace lib { namespace framework {

Wiimote::Wiimote()
{
   _proxy = new WiimoteProxy();
   _ei = 0;
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

size_t Wiimote::consumeEvents(std::list< WiimoteEvent >* pOut)
{
   if (! _proxy) { return 0; }
   WiimoteProxy* proxy = static_cast< WiimoteProxy* >(_proxy);

   if (! proxy->isMotionPlusEnabled()) { return 0; }
   size_t r = proxy->consumeEvents(pOut);
   if (r == 0) { return 0; }

   int ei = _ei ^ 1;
   _ev[ei] = pOut->back();
   _ei = ei;
   return r;
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
