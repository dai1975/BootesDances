#include <bootes/cegui/WindowDialog.h>
#include <bootes/lib/util/TChar.h>

namespace bootes { namespace cegui {

const CEGUI::String WindowDialog::EventSubmit    = "BootesCeguiWindowDialog_EventSubmit";
const CEGUI::String WindowDialog::EventCancel    = "BootesCeguiWindowDialog_EventCancel";
      CEGUI::String WindowDialog::WidgetTypeName = "BootesCeguiWindowDialog";

WindowDialog::WindowDialog(const CEGUI::String& type, const CEGUI::String& name)
   : ::CEGUI::FrameWindow(type, name)
{
   _pChild = NULL;
   _child_type = "";
   addEvent(WindowDialog::EventSubmit);
   addEvent(WindowDialog::EventCancel);
}

WindowDialog::~WindowDialog()
{
   if (_pChild) { delete _pChild; }
}

WindowDialog* WindowDialog::Create(const CEGUI::String& name, const CEGUI::String& childType)
{
   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();
   WindowDialog* p = static_cast< WindowDialog* >(wm.createWindow("WindowDialog", name));
   p->setChildType(childType);
   return p;
}

void WindowDialog::setChildType(const CEGUI::String& childType)
{
   _child_type = childType;
}

CEGUI::Window* WindowDialog::getChildWindow()
{
   return _pChild;
}

CEGUI::Window* WindowDialog::createChildWindow(const CEGUI::String& child_type)
{
   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();
   return wm.createWindow(child_type, "");
}

void WindowDialog::initialiseComponents()
{
   if (_pChild) { return; }

   setText("");
   setCloseButtonEnabled(false);
   //setTitleBarEnabled(false);

   _pChild = createChildWindow(_child_type);
   _pChild->initialiseComponents(); //Falagard 経由じゃないと呼ばれない?ので手動で
   _pChild->subscribeEvent(::bootes::cegui::WindowDialog::EventSubmit,
                           CEGUI::SubscriberSlot(&WindowDialog::onSubmit, this));
   _pChild->subscribeEvent(::bootes::cegui::WindowDialog::EventCancel,
                           CEGUI::SubscriberSlot(&WindowDialog::onCancel, this));
   this->addChildWindow(_pChild);

   {
      CEGUI::Size s0   = this->getParentPixelSize();
      CEGUI::Vector2 s = _pChild->getMinSize().asAbsolute(s0);
      CEGUI::UDim x(0,0), y(0,0);

      x.d_offset = s.d_x + 30;
      y.d_offset = s.d_y + 60;
      this->setMinSize(CEGUI::UVector2(x,y));
      x.d_offset = (s0.d_width  - x.d_offset) / 2;
      y.d_offset = (s0.d_height - y.d_offset) / 2;
      this->setPosition(CEGUI::UVector2(x,y));
   }
}

void WindowDialog::open()
{
   if (_pChild == NULL) { return; }
   //if (isVisible(true)) { return; }

   setAlwaysOnTop(true);
   setModalState(true);
   show();
}

bool WindowDialog::onSubmit(const CEGUI::EventArgs& ev)
{
   setModalState(false);
   hide();
   CEGUI::WindowEventArgs ev2(this);
   fireEvent(WindowDialog::EventSubmit, ev2);
   return true;
}

bool WindowDialog::onCancel(const CEGUI::EventArgs& ev)
{
   setModalState(false);
   hide();
   CEGUI::WindowEventArgs ev2(this);
   fireEvent(WindowDialog::EventCancel, ev2);
   return true;
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
