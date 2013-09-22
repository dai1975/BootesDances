#include "../include.h"
#include "BootesGame.h"
#include <bootes/lib/util/TChar.h>
#include "../view/RootViewPlay.h"
#include "../view/RootViewEdit.h"

BootesGame::BootesGame(const BootesGameOption& opt)
{
   m_state = S_0;
   _opt = opt;
   _pStageManager = NULL;
}

BootesGame::~BootesGame()
{
}

IStageManager* BootesGame::getStageManager()
{
   return _pStageManager;
}

void BootesGame::onUpdate(const ::bootes::lib::framework::GameTime* gt)
{
   typedef ::bootes::lib::framework::Game super;
   super::onUpdate(gt);

   switch (m_state) {
   case S_0:
      {
         RootView* p;
         if (_opt.editable) {
            p = new RootViewEdit(_opt.datadir.c_str());
         } else {
            p = new RootViewPlay(_opt.datadir.c_str());
         }
         _pStageManager = p->getStageManager();
         addView(p);
      }
      g_pFnd->getWiimote()->start();
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
