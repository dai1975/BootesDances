#include "ValueWindow.h"
#include <iostream>
#include <algorithm>
#include <bootes/cegui/DynamicFont.h>

namespace bootes { namespace dances { namespace editor {

const CEGUI::String ValueWindow::WidgetTypeName = "BootesDancesValueWindow";

ValueWindow::ValueWindow(
   const CEGUI::String& type, const CEGUI::String& name)
: CEGUI::LayoutContainer(type, name)
{
}

ValueWindow::~ValueWindow()
{
}

namespace {
/*
inline CEGUI::HorizontalLayoutContainer* CreateHBox(CEGUI::WindowManager& wm, const char* name) {
   CEGUI::HorizontalLayoutContainer* w =
      static_cast< CEGUI::HorizontalLayoutContainer* >(wm.createWindow("HorizontalLayoutContainer", name));
}
*/
   inline CEGUI::DefaultWindow* CreateLabel(CEGUI::WindowManager& wm, const char* name, CEGUI::Font* font)
   {
      CEGUI::DefaultWindow* w =
         static_cast< CEGUI::DefaultWindow* >(wm.createWindow("TaharezLook/StaticText", name));
      w->setProperty("FrameEnabled", "false");
      w->setFont(font);
      return w;
   }
}

bool ValueWindow::init()
{
   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();
   CEGUI::Renderer* pr = CEGUI::System::getSingleton().getRenderer();

   CEGUI::Font* font = ::bootes::cegui::DynamicFont::GetAsciiFont(4);

   {
      _pAccelX   = CreateLabel(wm, "AccelX", font);
      this->addChildWindow(_pAccelX);

      _pAccelY   = CreateLabel(wm, "AccelY", font);
      this->addChildWindow(_pAccelY);

      _pAccelZ   = CreateLabel(wm, "AccelZ", font);
      this->addChildWindow(_pAccelZ);
   }
   {
      _pGyroYaw   = CreateLabel(wm, "GyroYaw", font);
      this->addChildWindow(_pGyroYaw);

      _pGyroPitch = CreateLabel(wm, "GyroPitch", font);
      this->addChildWindow(_pGyroPitch);

      _pGyroRoll  = CreateLabel(wm, "GyroRoll", font);
      this->addChildWindow(_pGyroRoll);

   }
   {
      _pOrienYaw   = CreateLabel(wm, "OrienYaw", font);
      this->addChildWindow(_pOrienYaw);

      _pOrienPitch = CreateLabel(wm, "OrienPitch", font);
      this->addChildWindow(_pOrienPitch);

      _pOrienRoll  = CreateLabel(wm, "OrienRoll", font);
      this->addChildWindow(_pOrienRoll);
   }
   {
      _pButton = CreateLabel(wm, "Button", font);
      this->addChildWindow(_pButton);
   }

   onWiimote(NULL);
   return true;
}

void ValueWindow::layout()
{
   CEGUI::UDim x(0,0);
   CEGUI::UDim y(0,0);
   CEGUI::Size size = getPixelSize();

   {
      CEGUI::UDim w(0, LABEL_WIDTH);
      CEGUI::UDim h(0, LABEL_HEIGHT);

      {
         x.d_offset = 0;

         _pButton->setSize(CEGUI::UVector2(w,h));
         _pButton->setPosition(CEGUI::UVector2(x,y));
         x.d_offset += w.d_offset;

         y.d_offset += h.d_offset;
      }

      {
         x.d_offset = 0;
         y.d_offset += SPACE_SIZE;

         _pAccelX->setSize(CEGUI::UVector2(w,h));
         _pAccelX->setPosition(CEGUI::UVector2(x,y));
         x.d_offset += w.d_offset;

         _pAccelY->setSize(CEGUI::UVector2(w,h));
         _pAccelY->setPosition(CEGUI::UVector2(x,y));
         x.d_offset += w.d_offset;

         _pAccelZ->setSize(CEGUI::UVector2(w,h));
         _pAccelZ->setPosition(CEGUI::UVector2(x,y));
         x.d_offset += w.d_offset;

         y.d_offset += h.d_offset;
      }

      {
         x.d_offset = 0;
         y.d_offset += SPACE_SIZE;

         _pGyroYaw->setSize(CEGUI::UVector2(w,h));
         _pGyroYaw->setPosition(CEGUI::UVector2(x,y));
         x.d_offset += w.d_offset;

         _pGyroPitch->setSize(CEGUI::UVector2(w,h));
         _pGyroPitch->setPosition(CEGUI::UVector2(x,y));
         x.d_offset += w.d_offset;

         _pGyroRoll->setSize(CEGUI::UVector2(w,h));
         _pGyroRoll->setPosition(CEGUI::UVector2(x,y));
         x.d_offset += w.d_offset;

         y.d_offset += h.d_offset;
      }

      {
         x.d_offset = 0;
         y.d_offset += SPACE_SIZE;

         _pOrienYaw->setSize(CEGUI::UVector2(w,h));
         _pOrienYaw->setPosition(CEGUI::UVector2(x,y));
         x.d_offset += w.d_offset;

         _pOrienPitch->setSize(CEGUI::UVector2(w,h));
         _pOrienPitch->setPosition(CEGUI::UVector2(x,y));
         x.d_offset += w.d_offset;

         _pOrienRoll->setSize(CEGUI::UVector2(w,h));
         _pOrienRoll->setPosition(CEGUI::UVector2(x,y));
         x.d_offset += w.d_offset;

         y.d_offset += h.d_offset;
      }
   }
}

void ValueWindow::onWiimote(const ::bootes::lib::framework::WiimoteEvent* ev)
{
   char buf[128];

   if (ev != NULL && ev->_bConnect) {
      int i = 0;
      buf[i++] = (ev->isPressedA())? 'A': ' ';
      buf[i++] = (ev->isPressedB())? 'B': ' ';
      buf[i++] = (ev->isPressedPlus())? '+': ' ';
      buf[i++] = (ev->isPressedHome())? 'H': ' ';
      buf[i++] = (ev->isPressedMinus())? '-': ' ';
      buf[i++] = (ev->isPressed1())? '1': ' ';
      buf[i++] = (ev->isPressed2())? '2': ' ';
      buf[i++] = (ev->isPressedUp())? 'u': ' ';
      buf[i++] = (ev->isPressedDown())? 'd': ' ';
      buf[i++] = (ev->isPressedLeft())? 'l': ' ';
      buf[i++] = (ev->isPressedRight())? 'r': ' ';
      buf[i++] = '\0';
      _pButton->setText(buf);
   } else {
      //                 AB+H-12udlr
      _pButton->setText("no wiimote ");
   }

   if (ev != NULL && ev->_bConnect) {
      _snprintf_s(buf, 128, _TRUNCATE, "X: %.2f", ev->_accel.x);
      _pAccelX->setText(buf);
      _snprintf_s(buf, 128, _TRUNCATE, "Y: %.2f", ev->_accel.y);
      _pAccelY->setText(buf);
      _snprintf_s(buf, 128, _TRUNCATE, "Z: %.2f", ev->_accel.z);
      _pAccelZ->setText(buf);
   } else {
      _pAccelX->setText("X: --");
      _pAccelY->setText("Y: --");
      _pAccelZ->setText("Z: --");
   }

   if (ev != NULL && ev->_bMplus) {
      _snprintf_s(buf, 128, _TRUNCATE, "Y: %.2f", ev->_gyro.yaw);
      _pGyroYaw->setText(buf);
      _snprintf_s(buf, 128, _TRUNCATE, "P: %.2f", ev->_gyro.pitch);
      _pGyroPitch->setText(buf);
      _snprintf_s(buf, 128, _TRUNCATE, "R: %.2f", ev->_gyro.roll);
      _pGyroRoll->setText(buf);

      _snprintf_s(buf, 128, _TRUNCATE, "Y: %.2f", ev->_orien.yaw);
      _pOrienYaw->setText(buf);
      _snprintf_s(buf, 128, _TRUNCATE, "P: %.2f", ev->_orien.pitch);
      _pOrienPitch->setText(buf);
      _snprintf_s(buf, 128, _TRUNCATE, "R: %.2f", ev->_orien.roll);
      _pOrienRoll->setText(buf);
   } else {
      _pGyroYaw->setText("Y: --");
      _pGyroPitch->setText("P: --");
      _pGyroRoll->setText("R: --");

      _pOrienYaw->setText("Y: --");
      _pOrienPitch->setText("P: --");
      _pOrienRoll->setText("R: --");
   }

}

} } }

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
