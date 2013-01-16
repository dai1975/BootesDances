#ifndef BOOTESDANCES_CEGUI_DIALOGWINDOW_H
#define BOOTESDANCES_CEGUI_DIALOGWINDOW_H

#include <CEGUI.h>

namespace bootes { namespace cegui {

class DialogWindow: public CEGUI::FrameWindow
{
   typedef ::CEGUI::FrameWindow super;
public:
   static CEGUI::String WidgetTypeName;
   static const CEGUI::String EventDialog;

   DialogWindow(const CEGUI::String& type, const CEGUI::String& name);
   virtual ~DialogWindow();
   static DialogWindow* Create(const CEGUI::String& name);
   virtual void initialiseComponents();
   bool onPush(const ::CEGUI::EventArgs& e);

public:
   void setMessage(const char* msg);
   bool addButton(const char* msg, int wiimote_button);

   virtual void dialogBegin();
   virtual void dialogEnd();
   int getDialogedIndex() const;

protected:
   virtual ::CEGUI::Window* createChildWindow();

private:
   void* _pData;
   int _pushed;
};

} }

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
