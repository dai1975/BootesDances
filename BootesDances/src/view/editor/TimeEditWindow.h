#ifndef BOOTESDANCES_VIEW_CEGUITIMEEDITWINDOW_H
#define BOOTESDANCES_VIEW_CEGUITIMEEDITWINDOW_H

#include "../../include.h"
#include <bootes/dances/EvStage.h>
#include <map>
#include <string>
#include "TimeGrid.h"

#pragma warning(disable : 4995)
#include <CEGUI.h>

namespace bootes { namespace dances { namespace editor {

class TimeEditWindow
   : public CEGUI::LayoutContainer
   , public ::bootes::lib::framework::EventListener
{
public:
   static const CEGUI::String WidgetTypeName;

   TimeEditWindow(const CEGUI::String& type, const CEGUI::String& name);
   virtual ~TimeEditWindow();
   bool init();
   virtual void layout();

public:
   bool onScalePlus(const CEGUI::EventArgs&);
   bool onScaleMinus(const CEGUI::EventArgs&);
   virtual void onEvent(const ::bootes::lib::framework::Event* ev);
protected:
   void onLoad(const EvLoadStageResult* ev);

public:
   enum {
      TIMEEDIT_CONTROL_HEIGHT = 20,
      TIMEEDIT_GRID_HEIGHT = 100,
      TIMEEDIT_HEIGHT = TIMEEDIT_CONTROL_HEIGHT + TIMEEDIT_GRID_HEIGHT,
   };

private:
   TimeGrid* _grid;
   CEGUI::ScrollablePane* _scrollablePane;
   //CEGUI::Slider* _scaleSlider; //HorizontalはLookNFeel書き換えないとダメみたい
   CEGUI::PushButton *_scalePlusButton, *_scaleMinusButton;
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
