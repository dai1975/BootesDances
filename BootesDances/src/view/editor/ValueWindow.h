#ifndef BOOTESDANCES_VIEW_CEGUIVALUEWINDOW_H
#define BOOTESDANCES_VIEW_CEGUIVALUEWINDOW_H

#pragma warning(disable : 4819)
#include "../../include.h"
#include <map>
#include <string>

#pragma warning(disable : 4995)
#include <CEGUI.h>

namespace bootes { namespace dances { namespace editor {

class ValueWindow
   : public CEGUI::LayoutContainer
{
public:
   static const CEGUI::String WidgetTypeName;

   ValueWindow(const CEGUI::String& type, const CEGUI::String& name);
   virtual ~ValueWindow();
   bool init();
   virtual void layout();

public:
   void onWiimote(const ::bootes::lib::framework::WiimoteEvent* ev);

protected:

public:
   enum {
      MARGIN_SIZE = 5,
      SPACE_SIZE = 5,
      LABEL_WIDTH = 50,
      LABEL_HEIGHT = 24,
   };

private:
   //CEGUI::HorizontalLayoutContainer *_pAccelBox, *_pGyroBox;
   CEGUI::DefaultWindow *_pAccelX, *_pAccelY, *_pAccelZ;
   CEGUI::DefaultWindow *_pGyroYaw, *_pGyroPitch, *_pGyroRoll;
   CEGUI::DefaultWindow *_pOrienYaw, *_pOrienPitch, *_pOrienRoll;
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
