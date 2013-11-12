#include "../include.h"
#include <bootes/cegui/MessageDialog.h>
#include <bootes/cegui/DynamicFont.h>
#include <bootes/lib/util/TChar.h>

namespace bootes { namespace cegui {

CEGUI::String MessageWindow::WidgetTypeName = "BootesMessageWindow";

MessageWindow::MessageWindow(const CEGUI::String& type, const CEGUI::String& name)
  : CEGUI::LayoutContainer(type, name)
{
   addEvent(WindowDialog::EventSubmit);
   addEvent(WindowDialog::EventCancel);
   _buttons.reserve(10);
   _buttons.resize(0);
   _state = S_INIT;
}

MessageWindow::~MessageWindow()
{
#define SAFE_DEL(p) if (p) { delete p; p = NULL; }
   SAFE_DEL(_pMessageWindow);
   for (size_t i=0; i<_buttons.size(); ++i) {
      SAFE_DEL(_buttons[i].pButton);
   }
#undef SAFE_DEL
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

void MessageWindow::initialiseComponents()
{
   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();
   setText("");

   {
      _pMessageWindow = static_cast< CEGUI::DefaultWindow* >(wm.createWindow("TaharezLook/StaticText",""));
      _pMessageWindow->setFont(::bootes::cegui::DynamicFont::GetAsciiFont(8));
      _pMessageWindow->setProperty("FrameEnabled", "false");
      addChildWindow(_pMessageWindow);
   }

   setMinSize(CEGUI::UVector2(CEGUI::UDim(0,MIN_WIDTH), CEGUI::UDim(0,MIN_HEIGHT)));
   setUpdateMode(CEGUI::WUM_VISIBLE); //default
/*
   {
      CEGUI::Size s0   = this->getParentPixelSize();
      CEGUI::Vector2 s = pChild->getMinSize().asAbsolute(s0);
      CEGUI::UDim x(0,0), y(0,0);

      x.d_offset = s.d_x + 30;
      y.d_offset = s.d_y + 60;
      this->setMinSize(CEGUI::UVector2(x,y));
      x.d_offset = (s0.d_width  - x.d_offset) / 2;
      y.d_offset = (s0.d_height - y.d_offset) / 2;
      this->setPosition(CEGUI::UVector2(x,y));
   }
*/
}

void MessageWindow::layout()
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
         if (_pMessageWindow) {
            _pMessageWindow->setArea(CEGUI::UVector2(x,y), CEGUI::UVector2(w,h));
         }
      }
   }
}

void MessageWindow::setup()
{
/*
   for (int i=0; i<_buttons.size(); ++i) {
      _buttons[i].wiimote_pressed = false;
   }
*/
   _pushed = -1;
   _state = S_WAIT_RELEASE;
   _ev0.clear();
}
void MessageWindow::teardown()
{
   _state = S_INIT;

   CEGUI::WindowEventArgs ev(this);
   fireEvent(WindowDialog::EventSubmit, ev);
}

void MessageWindow::update(float elapsed)
{
   typedef CEGUI::LayoutContainer super;
   super::update(elapsed);

   if (_state != S_WAIT_RELEASE && _state != S_WAIT_PRESS) { return; }

   ::bootes::lib::framework::Wiimote* pWiimote = g_pFnd->getWiimote();
   if (pWiimote == NULL || !pWiimote->isConnected()) { return; }

   const ::bootes::lib::framework::WiimoteEvent* ev = pWiimote->getEvent();
   if (_state == S_WAIT_RELEASE) {
      for (size_t i=0; i<_buttons.size(); ++i) {
        ButtonEntry& be = _buttons[i];
         if (be.pButton == NULL || be.wiimote_button < 0) { continue; }

         if (ev->isPressed(be.wiimote_button)) { return; }
      }
      _ev0 = *ev;
      _state = S_WAIT_PRESS;

   } else if (_state == S_WAIT_PRESS) {
      ButtonEntry* p = NULL;
      for (size_t i=0; i<_buttons.size(); ++i) {
         ButtonEntry& be = _buttons[i];
         if (be.pButton == NULL || be.wiimote_button < 0) { continue; }

         if (!_ev0.isPressed(be.wiimote_button) && ev->isPressed(be.wiimote_button)) {
            p = &be;
            break;
         }
      }
      _ev0 = *ev;
      if (p) {
         _state = S_COMPLETE;
         CEGUI::WindowEventArgs e(p->pButton);
         p->pButton->fireEvent(::CEGUI::PushButton::EventClicked, e);
      }
   }
}

bool MessageWindow::onPush(const ::CEGUI::EventArgs& e)
{
   if (_state == S_INIT) { return true; }

   int i;
   {
      const ::CEGUI::WindowEventArgs& we = static_cast< const ::CEGUI::WindowEventArgs& >(e);
      for (i=0; i<_buttons.size(); ++i) {
         if (_buttons[i].pButton == we.window) { break; }
      }
      if (i == _buttons.size()) { return true; }
   }

   _pushed = i;
   teardown();
   return true;
}

void MessageWindow::setMessage(const char* msg)
{
   if (_pMessageWindow == NULL) { return; }
   _pMessageWindow->setText(msg);
}

bool MessageWindow::addButton(int id, const char* msg, int wiimote_button)
{
   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();

   if (_buttons.size() == _buttons.capacity()) {
      _buttons.reserve( _buttons.capacity() * 2 );
   }
   _buttons.resize( _buttons.size() + 1 );

   ButtonEntry& e = _buttons.back();
   e.id = id;
   e.wiimote_button = wiimote_button;
//   e.wiimote_pressed = false;
   e.pButton = static_cast< ::CEGUI::PushButton* >(wm.createWindow("TaharezLook/Button", ""));
   e.pButton->setText(msg);
   e.pButton->setFont(::bootes::cegui::DynamicFont::GetAsciiFont(8));

   addChildWindow(e.pButton);
   e.pButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::SubscriberSlot(&MessageWindow::onPush, this));

   return true;
}

int MessageWindow::getDialogedButton() const
{
   if (_pushed < 0 || _buttons.size() <= _pushed) { return -1; }
   return _buttons[ _pushed ].id;
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
