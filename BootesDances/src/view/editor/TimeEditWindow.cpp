#include "TimeEditWindow.h"
#include <iostream>
#include <algorithm>

namespace bootes { namespace dances { namespace editor {

const CEGUI::String TimeEditWindow::WidgetTypeName = "BootesDancesTimeEditWindow";


TimeEditWindow::TimeEditWindow(
   const CEGUI::String& type, const CEGUI::String& name)
: CEGUI::LayoutContainer(type, name)
{
   g_pFnd->getEventManager()->subscribe< EvLoadStageResult >(this);
//   setUsingAutoRenderingSurface(false);
   _grid = NULL;
   _scrollablePane = NULL;
}

TimeEditWindow::~TimeEditWindow()
{
   if (_scalePlusButton) { delete _scalePlusButton; }
   if (_scaleMinusButton) { delete _scaleMinusButton; }
   if (_scrollablePane) { delete _scrollablePane; }
   if (_grid) { delete _grid; }
}

bool TimeEditWindow::init()
{
   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();

   {
      _scaleMinusButton = static_cast< CEGUI::PushButton* >(wm.createWindow("TaharezLook/Button", "ScaleMinusButton"));
      _scaleMinusButton->setText("-");
      _scaleMinusButton->subscribeEvent(CEGUI::PushButton::EventClicked,
         CEGUI::Event::Subscriber(&TimeEditWindow::onScaleMinus, this));
      this->addChildWindow(_scaleMinusButton);

      _scalePlusButton = static_cast< CEGUI::PushButton* >(wm.createWindow("TaharezLook/Button", "ScalePlusButton"));
      _scalePlusButton->setText("+");
      _scalePlusButton->subscribeEvent(CEGUI::PushButton::EventClicked,
         CEGUI::Event::Subscriber(&TimeEditWindow::onScalePlus, this));
      this->addChildWindow(_scalePlusButton);
   }

   {
      _scrollablePane = static_cast< CEGUI::ScrollablePane* >(wm.createWindow("TaharezLook/ScrollablePane", "GridSP"));
      _scrollablePane->setShowHorzScrollbar(true);
      _scrollablePane->setShowVertScrollbar(false);
      this->addChildWindow(_scrollablePane);

      _grid = new TimeGrid( TimeGrid::WidgetTypeName, "grid");
      if (! _grid->init()) { return false; }
      _scrollablePane->addChildWindow(_grid);
   }

   return true;
}

void TimeEditWindow::layout()
{
   CEGUI::Rect rect = getUnclippedInnerRect();
   CEGUI::Size size;
   size.d_width  = rect.d_right - rect.d_left;
   size.d_height = rect.d_bottom - rect.d_top;
   //CEGUI::Size size = getPixelSize();
   int y0 = 0;

   /*
   {
   CEGUI::UDim ux(0,0);
   CEGUI::UDim uy(0,0);
   CEGUI::UDim uw(0,0);
   CEGUI::UDim uh(0,0);
   uw.d_offset = size.d_width - 20;
      uy.d_offset += 5;
      uh.d_offset += TIMEEDIT_CONTROL_HEIGHT -10;
      CEGUI::UDim w(0,16);
      CEGUI::UDim h(0,16);
      CEGUI::UDim x(ux);
      CEGUI::UDim y(0, uy.d_offset + (uh.d_offset-h.d_offset)/2);
      _scaleMinusButton->setPosition(CEGUI::UVector2(x,y));
      _scaleMinusButton->setSize(CEGUI::UVector2(w, h));
      x.d_offset += w.d_offset + 5;
      _scalePlusButton->setPosition(CEGUI::UVector2(x,y));
      _scalePlusButton->setSize(CEGUI::UVector2(w, h));
      uy.d_offset += uh.d_offset + 5;
   }
   {
      uy.d_offset += 5;
      uh.d_offset += TIMEEDIT_GRID_HEIGHT -10;
      _scrollablePane->setPosition(CEGUI::UVector2(ux,uy));
      _scrollablePane->setSize(CEGUI::UVector2(uw,uh));

      {
         CEGUI::Rect area = _scrollablePane->getViewableArea();
         CEGUI::UDim mw(0, area.getWidth());
         CEGUI::UDim mh(0, area.getHeight());
         _grid->setMinSize(CEGUI::UVector2(mw, mh));
         _grid->setMaxSize(CEGUI::UVector2(CEGUI::UDim(0,10000), CEGUI::UDim(0,10000)));
//         _grid->resize();
      }
      uy.d_offset += uh.d_offset + 5;
   }
   */
   {
      CEGUI::UDim w(0,16);
      CEGUI::UDim h(0,16);
      int dy = (TIMEEDIT_CONTROL_HEIGHT - h.d_offset) / 2;
      CEGUI::UDim x(0, 10);
      CEGUI::UDim y(0, y0 + dy);
      _scaleMinusButton->setPosition(CEGUI::UVector2(x,y));
      _scaleMinusButton->setSize(CEGUI::UVector2(w, h));

      x.d_offset += w.d_offset + 5;
      _scalePlusButton->setPosition(CEGUI::UVector2(x,y));
      _scalePlusButton->setSize(CEGUI::UVector2(w, h));

      y0 += TIMEEDIT_CONTROL_HEIGHT;
   }
   {
      CEGUI::UDim x(0, 10);
      CEGUI::UDim y(0, y0 + 5);
      CEGUI::UDim w(0, size.d_width - 20);
      CEGUI::UDim h(0, TIMEEDIT_GRID_HEIGHT - 10);
      //CEGUI::UDim h(0, size.d_height - y0 - 20);
      _scrollablePane->setPosition(CEGUI::UVector2(x,y));
      _scrollablePane->setSize(CEGUI::UVector2(w,h));

      {
         CEGUI::Rect area = _scrollablePane->getViewableArea();
         CEGUI::UDim mw(0, area.getWidth());
         CEGUI::UDim mh(0, area.getHeight());
         _grid->setMinSize(CEGUI::UVector2(mw, mh));
         _grid->setMaxSize(CEGUI::UVector2(CEGUI::UDim(0,10000), CEGUI::UDim(0,10000)));
         //_grid->resize();
      }
      y0 += TIMEEDIT_GRID_HEIGHT;
   }
}


void TimeEditWindow::onEvent(const ::bootes::lib::framework::GameTime* gt, const ::bootes::lib::framework::Event* ev)
{
   if (tryDispatch(ev, &TimeEditWindow::onLoad)) { return; }
}

void TimeEditWindow::onLoad(const EvLoadStageResult* ev)
{
   if (ev->_result) {
      _grid->onLoad(ev->_videoInfo);
      _scrollablePane->setHorizontalScrollPosition(0.0f);
   }
}

namespace {

struct ScaleSave
{
   float w0;
   float x0;

   void preScale(CEGUI::ScrollablePane* pane, TimeGrid* grid)
   {
      w0  = grid->getPixelSize().d_width;
      float r  = pane->getHorizontalScrollPosition();
      x0 = w0*r;
   }
   void postScale(CEGUI::ScrollablePane* pane, TimeGrid* grid)
   {
      float w1  = grid->getPixelSize().d_width;
      float x1 = x0 * w1 / w0;
      float r  = x1 / w1;
      pane->setHorizontalScrollPosition(r);
   }
};
}

bool TimeEditWindow::onScalePlus(const CEGUI::EventArgs& ev)
{
   ScaleSave save;
   save.preScale(_scrollablePane, _grid);
   unsigned int r = _grid->getResolution();
   _grid->setResolution(r / 2);
   save.postScale(_scrollablePane, _grid);
   return true;
}

bool TimeEditWindow::onScaleMinus(const CEGUI::EventArgs& ev)
{
   ScaleSave save;
   save.preScale(_scrollablePane, _grid);
   unsigned int r = _grid->getResolution();
   _grid->setResolution(r * 2);
   save.postScale(_scrollablePane, _grid);
   return true;
}

} } }

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
