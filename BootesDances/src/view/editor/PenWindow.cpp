#include "PenWindow.h"
#include "EditorDefines.h"
#include <iostream>
#include <algorithm>

namespace bootes { namespace dances { namespace editor {

const CEGUI::String PenWindow::WidgetTypeName = "BootesDancesPenWindow";

PenWindow::PenWindow(
   const CEGUI::String& type, const CEGUI::String& name)
: CEGUI::LayoutContainer(type, name)
{
   g_pFnd->getEventManager()->subscribe< EvEditorPen >(this);

   _button.resize(EditPen::NUM_PENS);
   _button[EditPen::PEN_ACTION].resize(EditPen::NUM_ACTIONS);
   _button[EditPen::PEN_NEWMOVE].resize(EditPen::NUM_MOVES);
   for (size_t i=0; i<_button.size(); ++i) {
      for (size_t j=0; j<_button[i].size(); ++j) {
         _button[i][j] = NULL;
      }
   }
}

PenWindow::~PenWindow()
{
   for (size_t i=0; i<_button.size(); ++i) {
      for (size_t j=0; j<_button[i].size(); ++j) {
         if (_button[i][j]) {
            delete _button[i][j];
         }
      }
   }
}

bool PenWindow::init()
{
   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();
   CEGUI::Renderer* pr = CEGUI::System::getSingleton().getRenderer();

#define CREATE_BUTTON(I,J,NAME) {\
   _button[I][J] = static_cast< CEGUI::PushButton* >(\
     wm.loadWindowLayout(NAME));\
   _button[I][J]->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&PenWindow::onPush, this));\
   this->addChildWindow(_button[I][J]);\
   }
   CREATE_BUTTON(EditPen::PEN_ACTION,  EditPen::ACTION_DEFAULT, "DefaultPenButton.layout");
   CREATE_BUTTON(EditPen::PEN_NEWMOVE, EditPen::MOVE_LINE,      "LineButton.layout");
   CREATE_BUTTON(EditPen::PEN_NEWMOVE, EditPen::MOVE_ELLIPSE,   "EllipseButton.layout");
   CREATE_BUTTON(EditPen::PEN_NEWMOVE, EditPen::MOVE_SPLINE,    "SplineButton.layout");
#undef CREATE_BUTTON

   {
      EvEditorPen ev;
      ev._main = EditPen::PEN_ACTION;
      ev._sub  = EditPen::ACTION_DEFAULT;
      g_pFnd->getEventManager()->queue(&ev);
   }

   return true;
}

void PenWindow::layout()
{
   CEGUI::UDim ux(0,10);
   CEGUI::UDim uy(0,0);
   CEGUI::UDim uw(0,0);
   CEGUI::UDim uh(0,0);

   CEGUI::Size size = getPixelSize();

   {
      CEGUI::UVector2 bsize;
      bsize.d_x = CEGUI::UDim(0,BUTTON_SIZE);
      bsize.d_y = CEGUI::UDim(0,BUTTON_SIZE);

      const int cellsize = (BUTTON_SIZE + SPACE_SIZE);
      int nx = (int)(size.d_width - (2*MARGIN_SIZE)) / cellsize;
      if (nx < 1) { nx = 1; }

      int xi = 0;
      int yi = 0;
      CEGUI::UDim x(0,0),y(0,0);
      for (size_t i=0; i<_button.size(); ++i) {
         for (size_t j=0; j<_button[i].size(); ++j) {
            x.d_offset = MARGIN_SIZE + (0.5f * SPACE_SIZE) + xi*cellsize;
            y.d_offset = MARGIN_SIZE + (0.5f * SPACE_SIZE) + yi*cellsize;
            _button[i][j]->setSize(bsize);
            _button[i][j]->setPosition(CEGUI::UVector2(x,y));

            xi++;
            if (xi == nx) {
               xi = 0;
               yi++;
            }
         }
         yi++;
         xi = 0;
      }
   }
}

void PenWindow::onEvent(const ::bootes::lib::framework::GameTime* gt, const ::bootes::lib::framework::Event* ev)
{
   if (tryDispatch(ev, &PenWindow::onPen)) { return; }
}

bool PenWindow::onPush(const CEGUI::EventArgs& args)
{
   const CEGUI::WindowEventArgs& ev =
      static_cast< const CEGUI::WindowEventArgs& >(args);

   for (size_t i=0; i<_button.size(); ++i) {
      for (size_t j=0; j<_button[i].size(); ++j) {
         if (_button[i][j] == ev.window) {
            EvEditorPen ev;
            ev._main = i;
            ev._sub  = j;
            g_pFnd->getEventManager()->deliver(g_pFnd->getGameTime(), &ev);
            break;
         }
      }
   }

   return true;
}

void PenWindow::onPen(const EvEditorPen* ev)
{
   for (size_t i=0; i<_button.size(); ++i) {
      for (size_t j=0; j<_button[i].size(); ++j) {
         if (ev->_main == i && ev->_sub == j) {
            _button[i][j]->setProperty("Selected", "true");
         } else {
            _button[i][j]->setProperty("Selected", "false");
         }
      }
   }
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
