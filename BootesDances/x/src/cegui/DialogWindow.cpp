#include <bootes/cegui/DialogWindow.h>
#include <bootes/cegui/DynamicFont.h>
#include "DialogWindowLayout.h"

namespace bootes { namespace cegui {

const CEGUI::String DialogWindow::EventDialog    = "BootesCeguiDialogWindow_EventSubmit";
      CEGUI::String DialogWindow::WidgetTypeName = "BootesCeguiDialogWindow";

DialogWindow::DialogWindow(const CEGUI::String& type, const CEGUI::String& name)
   : ::CEGUI::FrameWindow(type, name)
{
   addEvent(DialogWindow::EventDialog);
   _pData = NULL;
   _pushed = -1;
}

DialogWindow::~DialogWindow()
{
   if (_pData) { delete static_cast< DialogWindowLayout* >(_pData); }
}

DialogWindow* DialogWindow::Create(const CEGUI::String& name)
{
   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();
   return static_cast< DialogWindow* >(wm.createWindow("DialogWindow", name));
}

::CEGUI::Window* DialogWindow::createChildWindow()
{
   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();
   CEGUI::DefaultWindow* p = static_cast< CEGUI::DefaultWindow* >(wm.createWindow("TaharezLook/StaticText",""));
   p->setFont(::bootes::cegui::DynamicFont::GetAsciiFont(8));
   p->setProperty("FrameEnabled", "false");
   return p;
}

void DialogWindow::initialiseComponents()
{
   if (_pData) { return; }

   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();

   DialogWindowLayout* pLayout = static_cast< DialogWindowLayout* >(wm.createWindow(DialogWindowLayout::WidgetTypeName, ""));
   pLayout->initialiseComponents(); //Falagard 経由じゃないと呼ばれないので手動で

   pLayout->setChildWindow(createChildWindow());
   this->addChildWindow(pLayout);

   _pData = pLayout;
   setText("");
   setCloseButtonEnabled(false);
   //setTitleBarEnabled(false);

   {
      CEGUI::Size s0   = this->getParentPixelSize();
      CEGUI::Vector2 s = pLayout->getMinSize().asAbsolute(s0);
      CEGUI::UDim x(0,0), y(0,0);

      x.d_offset = s.d_x + 30;
      y.d_offset = s.d_y + 60;
      this->setMinSize(CEGUI::UVector2(x,y));
      x.d_offset = (s0.d_width  - x.d_offset) / 2;
      y.d_offset = (s0.d_height - y.d_offset) / 2;
      this->setPosition(CEGUI::UVector2(x,y));
   }
}

void DialogWindow::setMessage(const char* msg)
{
   if (_pData == NULL) { return; }
   DialogWindowLayout* pLayout = static_cast< DialogWindowLayout* >(_pData);

   ::CEGUI::Window* pChild = pLayout->getChildWindow();
   if (pChild == NULL) { return; }

   pChild->setText(msg);
}

bool DialogWindow::addButton(const char* msg, int wiimote_button)
{
   if (_pData == NULL) { return false; }
   DialogWindowLayout* pLayout = static_cast< DialogWindowLayout* >(_pData);

   ::CEGUI::PushButton* p = pLayout->addButton(msg, wiimote_button);
   if (p == NULL) {
      return false;
   }
   p->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::SubscriberSlot(&DialogWindow::onPush, this));
   return true;
}

void DialogWindow::dialogBegin()
{
   if (_pData == NULL) { return; }
   DialogWindowLayout* pLayout = static_cast< DialogWindowLayout* >(_pData);

   {
      CEGUI::Size s0   = this->getParentPixelSize();
      CEGUI::Vector2 s = pLayout->getMinSize().asAbsolute(s0);
      CEGUI::UDim x(0,0), y(0,0);

      x.d_offset = s.d_x + 30;
      y.d_offset = s.d_y + 60;
      this->setMinSize(CEGUI::UVector2(x,y));
      x.d_offset = (s0.d_width  - x.d_offset) / 2;
      y.d_offset = (s0.d_height - y.d_offset) / 2;
      this->setPosition(CEGUI::UVector2(x,y));
   }
   pLayout->wiimoteCheckBegin();

   _pushed = -1;
   setAlwaysOnTop(true);
   setModalState(true);
   show();
}

void DialogWindow::dialogEnd()
{
   setModalState(false);
   hide();
}

bool DialogWindow::onPush(const ::CEGUI::EventArgs& e)
{
   if (_pData == NULL) { return false; }
   DialogWindowLayout* pLayout = static_cast< DialogWindowLayout* >(_pData);
   pLayout->wiimoteCheckEnd();

   const ::CEGUI::WindowEventArgs& we = static_cast< const ::CEGUI::WindowEventArgs& >(e);
   _pushed = pLayout->getButtonIndex(we.window);

   CEGUI::WindowEventArgs ev(this);
   fireEvent(DialogWindow::EventDialog, ev);
   return true;
}

int DialogWindow::getDialogedIndex() const
{
   return _pushed;
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
