#ifndef BOOTES_APP_BOOTESWIIMOTE_H
#define BOOTES_APP_BOOTESWIIMOTE_H

#include <bootes/lib/framework/Foundation.h>
#include <bootes/lib/framework/Wiimote.h>
#include <bootes/lib/framework/InputEvent.h>

class BootesWiimote: public bootes::lib::framework::Wiimote
{
   typedef ::bootes::lib::framework::WiimoteEvent WiimoteEvent;
public:
   BootesWiimote();

   inline bool isCalib() const { return _bCalib; }

protected:
   virtual bool calcData(WiimoteEvent* newev, const WiimoteEvent* oldev);

private:
   bool filter(WiimoteEvent* ev);
   WiimoteEvent _lpf;

   bool _bCalib;
   DWORD _tPaused;
   size_t _nCalib;
   WiimoteEvent::Orien _calib;
   WiimoteEvent::Orien _orien;
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
