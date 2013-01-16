#include "../include.h"
#include "BootesGame.h"
#include <bootes/lib/util/TChar.h>
#include "../view/RootViewPlay.h"
#include "../view/RootViewEdit.h"

BootesGame::BootesGame(const TCHAR* dir, bool editable)
{
   m_state = S_0;
   _dir = dir;
   _editable = editable;
   _pStageManager = NULL;
}

BootesGame::~BootesGame()
{
}

IStageManager* BootesGame::getStageManager()
{
   return _pStageManager;
}

/*
namespace {
dancebootes::lib::Stage* create_dummy_stage()
{
   dancebootes::lib::Stage* stage = new dancebootes::lib::Stage();
   stage->set_name("dummy stage");

   {
      dancebootes::lib::Movement* move = stage->add_moves();
      move->set_time0(0);
      move->set_time1(3*1000*10000);
      dancebootes::lib::Movement::Line* line = move->mutable_line();
      dancebootes::lib::Point* p = line->mutable_p0();
      p->set_x(0.1f);
      p->set_y(0.1f);
      p = line->mutable_p1();
      p->set_x(0.9f);
      p->set_y(0.9f);
   }

   {
      dancebootes::lib::Movement* move = stage->add_moves();
      move->set_time0(4*1000*10000);
      move->set_time1(7*1000*10000);

      dancebootes::lib::Movement::Ellipse* e = move->mutable_ellipse();
      dancebootes::lib::Point* p;
      p = e->mutable_center();
      p->set_x(0.5f);
      p->set_y(0.5f);

      p = e->mutable_radius();
      p->set_x(0.3f);
      p->set_y(0.2f);

      e->set_angle0(-3.14f * 0.4f);
      e->set_angle1(-3.14f * 0.6f);
      e->set_direction(true);
   }
   return stage;
}
}
*/

void BootesGame::onUpdate(double currentTime, int elapsedTime)
{
   switch (m_state) {
   case S_0:
      {
         RootView* p;
         if (_editable) {
            p = new RootViewEdit(_dir.c_str());
         } else {
            p = new RootViewPlay(_dir.c_str());
         }
         _pStageManager = p->getStageManager();
         addView(p);
      }
      g_pFnd->getWiimote()->start();
      /*
      {
         char* dir = ::bootes::lib::util::TChar::T2C(_dir.c_str());
         dancebootes::lib::Stage* stage = create_dummy_stage();
         stage->set_mypath(dir);
         stage->mutable_mypath()->append("\\demo.stg");
         stage->set_moviepath("demo.mp4");
         delete dir;

         EvLoadStage ev;
         ev._stage.reset(stage);
         g_pFnd->queue(&ev);
      }
      */
      m_state = S_RUN;
      break;

   case S_RUN:
      break;
   }
}


/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
