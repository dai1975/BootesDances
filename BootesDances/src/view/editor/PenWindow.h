#ifndef BOOTESDANCES_VIEW_CEGUIPENWINDOW_H
#define BOOTESDANCES_VIEW_CEGUIPENWINDOW_H

#include "../../include.h"
#include <bootes/dances/EvEditor.h>
#include <map>
#include <string>

#pragma warning(disable : 4995)
#include <CEGUI.h>

namespace bootes { namespace dances { namespace editor {

class PenWindow
   : public CEGUI::LayoutContainer
   , public ::bootes::lib::framework::EventListener
{
public:
   static const CEGUI::String WidgetTypeName;

   PenWindow(const CEGUI::String& type, const CEGUI::String& name);
   virtual ~PenWindow();
   bool init();
   virtual void layout();

public:
   virtual void onEvent(const ::bootes::lib::framework::Event* ev);
   bool onPush(const CEGUI::EventArgs&);
protected:
   void onPen(const EvEditorPen*);

public:
   enum {
      MARGIN_SIZE = 5,
      SPACE_SIZE = 5,
      BUTTON_SIZE = 24,
   };

private:
   std::vector< std::vector< CEGUI::PushButton* > > _button;
};

} } }

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
