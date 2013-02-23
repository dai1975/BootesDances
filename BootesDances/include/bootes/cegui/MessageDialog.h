#ifndef BOOTESDANCES_CEGUI_MESSAGEDIALOG_H
#define BOOTESDANCES_CEGUI_MESSAGEDIALOG_H

#include <CEGUI.h>
#include "WindowDialog.h"

namespace bootes { namespace cegui {

class MessageWindow: public CEGUI::LayoutContainer
{
public:
   static CEGUI::String WidgetTypeName;

   MessageWindow(const CEGUI::String& type, const CEGUI::String& name);
   virtual ~MessageWindow();
   virtual void initialiseComponents();
   virtual void layout();
   virtual void update(float elapsed);
   bool onPush(const ::CEGUI::EventArgs& e);

public:
   void setup();
   void setMessage(const char* msg);
   bool addButton(int id, const char* msg, int wiimote_button = 0);
   int  getDialogedButton() const;

private:
   void teardown();
   enum { S_INIT, S_WAIT_RELEASE, S_WAIT_PRESS, S_COMPLETE };
   int _state;

   struct ButtonEntry {
      int id;
      ::CEGUI::PushButton *pButton;
      int wiimote_button;
      //bool wiimote_pressed;
   };
   CEGUI::DefaultWindow* _pMessageWindow;
   std::vector< ButtonEntry > _buttons;
   int _pushed;
};

typedef Dialog< MessageWindow > MessageDialog;

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
