#ifndef BOOTESDANCES_CEGUI_DIALOGWINDOWLAYOUT_H
#define BOOTESDANCES_CEGUI_DIALOGWINDOWLAYOUT_H

#include "../include.h"
#include <CEGUI.h>

namespace bootes { namespace cegui {

class DialogWindowLayout: public CEGUI::LayoutContainer
{
   typedef CEGUI::LayoutContainer super;
public:
   static CEGUI::String WidgetTypeName;
   DialogWindowLayout(const CEGUI::String& type, const CEGUI::String& name);
   virtual ~DialogWindowLayout();
   virtual void initialiseComponents();

   virtual void layout();

   void setChildWindow(::CEGUI::Window*);
   ::CEGUI::Window* getChildWindow();
   ::CEGUI::PushButton* addButton(const char* text, int wiimote_id);

   int getButtonIndex(const ::CEGUI::Window*) const;

   void wiimoteCheckBegin();
   void wiimoteCheckEnd();
   virtual void update(float elapsed);

public:
   ::CEGUI::Window     *_pChildWindow;
   struct ButtonEntry {
      ::CEGUI::PushButton *pButton;
      int wiimote_button;
      bool wiimote_pressed;
   };
   std::vector< ButtonEntry > _buttons;
   ::bootes::lib::framework::WiimoteEvent _ev0;
   bool _check_wiimote;
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
