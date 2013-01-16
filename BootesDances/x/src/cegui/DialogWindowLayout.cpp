#include "DialogWindowLayout.h"
#include <bootes/cegui/DynamicFont.h>

namespace bootes { namespace cegui {

CEGUI::String DialogWindowLayout::WidgetTypeName = "BootesCeguiDialogWindowLayout";

DialogWindowLayout::DialogWindowLayout(const CEGUI::String& type, const CEGUI::String& name)
  : CEGUI::LayoutContainer(type, name)
{
   _pChildWindow  = NULL;
   _buttons.reserve(10);
   _buttons.resize(0);
   _check_wiimote = false;
}

DialogWindowLayout::~DialogWindowLayout()
{
#define SAFE_DEL(p) if (p) { delete p; p = NULL; }
   SAFE_DEL(_pChildWindow);
   for (size_t i=0; i<_buttons.size(); ++i) {
      SAFE_DEL(_buttons[i].pButton);
   }
#undef SAFE_DEL
}

void DialogWindowLayout::setChildWindow(::CEGUI::Window* p)
{
   _pChildWindow = p;
   addChildWindow(_pChildWindow);
}

::CEGUI::Window* DialogWindowLayout::getChildWindow()
{
   return _pChildWindow;
}

::CEGUI::PushButton* DialogWindowLayout::addButton(const char* text, int wiimote_id)
{
   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();

   if (_buttons.size() == _buttons.capacity()) {
      _buttons.reserve( _buttons.capacity() * 2 );
   }
   _buttons.resize( _buttons.size() + 1 );

   ButtonEntry& e = _buttons.back();
   e.wiimote_button = wiimote_id;
   e.pButton = static_cast< ::CEGUI::PushButton* >(wm.createWindow("TaharezLook/Button", ""));
   e.pButton->setText(text);
   e.pButton->setFont(::bootes::cegui::DynamicFont::GetAsciiFont(8));
   addChildWindow(e.pButton);

   return e.pButton;
}

int DialogWindowLayout::getButtonIndex(const ::CEGUI::Window* p) const
{
   for (int i=0; i<_buttons.size(); ++i) {
      if (_buttons[i].pButton == p) { return i; }
   }
   return -1;
}

namespace {
   enum {
      SPACE_SIZE = 5,
      MAIN_MIN_HEIGHT = 100,

      TEXT_HEIGHT = 16,
      TEXT_WIDTH  = 8,
      BUTTON_HEIGHT = TEXT_HEIGHT * 1,
      BUTTON_WIDTH  = TEXT_WIDTH  * 10,

      MIN_HEIGHT = BUTTON_HEIGHT + MAIN_MIN_HEIGHT + 3*SPACE_SIZE,
      MIN_WIDTH  = SPACE_SIZE + 4*(BUTTON_WIDTH + SPACE_SIZE),
   };
}

void DialogWindowLayout::initialiseComponents()
{
   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();

   setMinSize(CEGUI::UVector2(CEGUI::UDim(0,MIN_WIDTH), CEGUI::UDim(0,MIN_HEIGHT)));
}

void DialogWindowLayout::layout()
{
   CEGUI::Size size = getPixelSize();

   {
      CEGUI::UDim x(0,0), y(0,0), w(0,0), h(0,0);
      int maxCols = (size.d_width - SPACE_SIZE) / (SPACE_SIZE + BUTTON_WIDTH);
      int nRows = (_buttons.size() + maxCols - 1) / maxCols;

      {
         for (int row=0; row<nRows; ++row) {
            int i0 = row * maxCols;
            int nCols = _buttons.size() - i0;
            if (maxCols < nCols) { nCols = maxCols; }

            int button_w0 = (size.d_width - SPACE_SIZE) / nCols;
            int button_w = button_w0 - SPACE_SIZE;
            if (BUTTON_WIDTH < button_w) { button_w = BUTTON_WIDTH; }

            w.d_offset = button_w;
            h.d_offset = BUTTON_HEIGHT;
            y.d_offset = size.d_height - SPACE_SIZE - (nRows-row)*BUTTON_HEIGHT; //bottom align
            for (int col=0; col<nCols; ++col) {
               ButtonEntry& be = _buttons[ i0 + col ];
               x.d_offset = size.d_width  - SPACE_SIZE - (nCols-col)*button_w0; //right align
               be.pButton->setArea(CEGUI::UVector2(x,y), CEGUI::UVector2(w,h));
            }
         }
      }

      {
         x.d_offset = SPACE_SIZE;
         y.d_offset = SPACE_SIZE;
         h.d_offset = size.d_height - 2*SPACE_SIZE - nRows * BUTTON_HEIGHT;
         w.d_offset = size.d_width  - 2*SPACE_SIZE;
         if (_pChildWindow) {
            _pChildWindow->setArea(CEGUI::UVector2(x,y), CEGUI::UVector2(w,h));
         }
      }
   }
}

void DialogWindowLayout::wiimoteCheckBegin()
{
   for (size_t i=0; i<_buttons.size(); ++i) {
      ButtonEntry& be = _buttons[i];
      be.wiimote_pressed = true;
   }
   _check_wiimote = true;
}
void DialogWindowLayout::wiimoteCheckEnd()
{
   _check_wiimote = false;
}

void DialogWindowLayout::update(float elapsed)
{
   super::update(elapsed);
   if (! _check_wiimote) { return; }

   ::bootes::lib::framework::Wiimote* pWiimote = g_pFnd->getWiimote();
   if (pWiimote == NULL || !pWiimote->isConnected()) { return; }

   ButtonEntry* p = NULL;
   for (size_t i=0; i<_buttons.size(); ++i) {
      ButtonEntry& be = _buttons[i];
      if (be.pButton == NULL || be.wiimote_button < 0) { continue; }

      bool b0 = be.wiimote_pressed;
      be.wiimote_pressed = pWiimote->isPressed(be.wiimote_button);

      if (p == NULL && !b0 && be.wiimote_pressed) {
         p = &be;
      }
   }
   if (p) {
      CEGUI::WindowEventArgs ev(p->pButton);
      p->pButton->fireEvent(::CEGUI::PushButton::EventClicked, ev);
   }
}

} }

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
