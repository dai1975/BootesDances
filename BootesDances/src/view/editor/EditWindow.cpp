#include "EditWindow.h"
#include <bootes/cegui/DynamicFont.h>
#include "../../move/Move.h"
#include "../../move/guide/GuideRibbonLine.h"
#include "../../move/guide/GUideRibbonEllipse.h"
#include "../../move/guide/GuideRibbonSpline.h"
#include <iostream>
#include <algorithm>

namespace bootes { namespace dances { namespace editor {

const CEGUI::String EditWindow::WidgetTypeName = "BootesDancesEditWindow";

EditWindow::EditWindow(const CEGUI::String& type, const CEGUI::String& name)
   : ::CEGUI::LayoutContainer(type, name)
{
   _videoInfo.size_100ns = 0;
   _videoInfo.tpf_100ns = 0;
   _isThumbTrack = false;
   _copied_move = NULL;
   _pen_main = EditPen::PEN_ACTION;
   _pen_sub  = EditPen::ACTION_DEFAULT;
   _pTeachMove = NULL;
   _teach_stage = TEACH_NONE;
   _play = false;
   g_pFnd->getEventManager()->subscribe< EvLoadStageResult >(this);
   g_pFnd->getEventManager()->subscribe< EvNewStageResult >(this);
   g_pFnd->getEventManager()->subscribe< EvMoviePlay >(this);
   g_pFnd->getEventManager()->subscribe< EvMoviePause >(this);
   g_pFnd->getEventManager()->subscribe< EvEditorPen >(this);
}

EditWindow::~EditWindow()
{
#define SAFE_DEL(PTR) { if (PTR) { delete PTR; } }
   SAFE_DEL(_copied_move);
   SAFE_DEL(_pFrameBar);
   SAFE_DEL(_pFrameText);
   SAFE_DEL(_pTimeText);
   SAFE_DEL(_pCtrl);
   SAFE_DEL(_pTimeEdit);
   SAFE_DEL(_pScreenMenu);
   SAFE_DEL(_pScreen);
   SAFE_DEL(_pPenWin);
   SAFE_DEL(_pValueWin);
   SAFE_DEL(_pWiimoteWin);
   SAFE_DEL(_pFrame);
   SAFE_DEL(_pTeachBeginDialog);
   SAFE_DEL(_pTeachResultDialog);
   SAFE_DEL(_pTeachContDialog);
#undef SAFE_DEL
}

void EditWindow::onEvent(const GameTime* gt, const ::bootes::lib::framework::Event* ev)
{
   if (ev->getEventId() == EvLoadStageResult::GetEventId()) {
      const EvLoadStageResult* e = static_cast< const EvLoadStageResult* >(ev);
      onLoad(e->_result, e->_videoInfo);
      return;
   } else if (ev->getEventId() == EvNewStageResult::GetEventId()) {
      const EvNewStageResult* e = static_cast< const EvNewStageResult* >(ev);
      onLoad(e->_result, e->_videoInfo);
      return;
   }
   else if (tryDispatch(ev, &EditWindow::onPlay)) { return; }
   else if (tryDispatch(ev, &EditWindow::onPause)) { return; }
   else if (tryDispatch(ev, &EditWindow::onPen)) { return; }
}

namespace {
   struct HMSMS {
      int h,m,s,ms;
      HMSMS(__int64 t) {
         t /= 10000; //ms
         this->ms = t % 1000;
         t /= 1000; //s
         this->s = t % 60;
         t /= 60;
         this->m = t % 60;
         this->h = static_cast< int >(t / 60);
      }
   };

void setFrameText(CEGUI::DefaultWindow* pFrameWnd, CEGUI::DefaultWindow* pTimeWnd,
                  __int64 max_100ns, __int64 current_100ns, __int64 tpf_100ns)
{
   char buf[128];

   if (pTimeWnd) {
      __int64 maxframe = (tpf_100ns == 0)? 0: max_100ns / tpf_100ns;
      __int64 frame    = (tpf_100ns == 0)? 0: current_100ns / tpf_100ns;
      _snprintf_s(buf, 128, _TRUNCATE, "%lld/%lld", frame, maxframe);
      pTimeWnd->setText(buf);
   }

   if (pFrameWnd) {
      HMSMS m(max_100ns);
      HMSMS c(current_100ns);
      _snprintf_s(buf, 128, _TRUNCATE,
                  "%3d:%02d:%02d.%03d/%3d:%02d:%02d.%03d",
                  c.h, c.m, c.s, c.ms,
                  m.h, m.m, m.s, m.ms);
      pFrameWnd->setText(buf);
   }
}

}

void EditWindow::onPen(const EvEditorPen* ev)
{
   _pen_main = ev->_main;
   _pen_sub  = ev->_sub;
}

void EditWindow::onLoad(bool result, const VideoInfo& vi)
{
   _videoInfo = vi;
   if (_pCtrl == NULL) { return; }

   _play = false;
   _pCtrlBtn[CTRL_PLAY]->setText(">");
   _pCtrlBtn[CTRL_PLAY]->setEnabled(result);

   if (result) {
      //_pSlider->setMaxValue((float)_evLoad._size_100ns);
      //_pSlider->setClickStep((float)_evLoad._tpf_100ns);
      __int64 max = _videoInfo.size_100ns;
      __int64 tpf = _videoInfo.tpf_100ns;
      _pFrameBar->setDocumentSize((float)(max + tpf));
      _pFrameBar->setPageSize((float)tpf);
      _pFrameBar->setStepSize((float)tpf);
      setFrameText(_pFrameText, _pTimeText, max, 0, tpf);
   } else {
      _pFrameBar->setDocumentSize(0);
      _pFrameBar->setPageSize(0);
      _pFrameBar->setStepSize(0);
      setFrameText(_pFrameText, _pTimeText, 0, 0, 1);
   }
   _pTeachMove = NULL;
   _teach_stage = TEACH_NONE;
}
void EditWindow::onPlay(const EvMoviePlay* ev)
{
   _pScreenMenu->hide();

   if (_pCtrl == NULL) { return; }
   _play = true;
   _pCtrlBtn[CTRL_PLAY]->setText("||");
   _pCtrlBtn[CTRL_BACK1]->disable();
   _pCtrlBtn[CTRL_FORWARD1]->disable();
}
void EditWindow::onPause(const EvMoviePause* ev)
{
   if (_pCtrl == NULL) { return; }
   _play = false;
   _pCtrlBtn[CTRL_PLAY]->setText(">");
   _pCtrlBtn[CTRL_BACK1]->enable();
   _pCtrlBtn[CTRL_FORWARD1]->enable();
}

bool EditWindow::doPlay(const CEGUI::EventArgs& e)
{
   if (_pCtrlBtn[CTRL_PLAY]->getText().at(0) == '>') {
      EvPlayMovie ev;
      g_pFnd->getEventManager()->queue(&ev);
   } else {
      EvPauseMovie ev;
      g_pFnd->getEventManager()->queue(&ev);
   }
   return true;
}
bool EditWindow::doSuspend(const CEGUI::EventArgs& e)
{
   _isThumbTrack = true;
   EvPauseMovie ev;
   g_pFnd->getEventManager()->queue(&ev);
   return true;
}
bool EditWindow::doResume(const CEGUI::EventArgs& e)
{
   _isThumbTrack = false;
   return true;
}
bool EditWindow::doBack(const CEGUI::EventArgs& e)
{
   EvSeekMovie ev(EvSeekMovie::SET, 0);
   g_pFnd->getEventManager()->queue(&ev);
   return true;
}
bool EditWindow::doForward(const CEGUI::EventArgs& e)
{
   EvSeekMovie ev(EvSeekMovie::END, 0);
   g_pFnd->getEventManager()->queue(&ev);
   return true;
}
bool EditWindow::doBack1(const CEGUI::EventArgs& e)
{
   __int64 tpf = _videoInfo.tpf_100ns;
   EvSeekMovie ev(EvSeekMovie::CUR, -tpf);
   g_pFnd->getEventManager()->queue(&ev);
   return true;
}
bool EditWindow::doForward1(const CEGUI::EventArgs& e)
{
   __int64 tpf = _videoInfo.tpf_100ns;
   EvSeekMovie ev(EvSeekMovie::CUR, tpf);
   g_pFnd->getEventManager()->queue(&ev);
   return true;
}
bool EditWindow::doFrame(const CEGUI::EventArgs& e)
{
   _pScreenMenu->hide();

   if (! _isThumbTrack) { return true; }
   if (_pFrameBar == NULL) { return true; }
   float f = _pFrameBar->getScrollPosition();
   EvSeekMovie ev(EvSeekMovie::SET, static_cast< __int64 >(f));
   g_pFnd->getEventManager()->queue(&ev);
   return true;
}

void EditWindow::onInput(const GameTime* gt, const ::bootes::lib::framework::InputEvent* ev)
{
   switch (ev->_type) {
   case ::bootes::lib::framework::InputEvent::T_WNDMSG:
      break;
   case ::bootes::lib::framework::InputEvent::T_WIIMOTE:
      {
         const ::bootes::lib::framework::WiimoteEvent* we = static_cast< const ::bootes::lib::framework::WiimoteEvent* >(ev);
         _pValueWin->onWiimote(we);
         onWiimote(we);
      }
   }
}

// todo: split to new class such as TeachController
void EditWindow::onWiimote(const ::bootes::lib::framework::WiimoteEvent* ev)
{
   if (_teach_stage == TEACH_NONE) {
      if (CEGUI::System::getSingleton().getModalTarget() != NULL) { return; }

      MoveEditee me;
      IMoveEditor* pEditor = g_pGame->getStageManager()->getMoveEditor();
      IWiimoteHandler* pWHandler = g_pGame->getStageManager()->getWiimoteHandler();
      if (pEditor == NULL || pWHandler == NULL) { return; }
      if (! pEditor->editeeSelected(&me)) { return; }

      if (ev->isPressed1()) {
         EvPauseMovie ev;
         g_pFnd->getEventManager()->queue(&ev);

         _teach_stage = TEACH_TEACHDIALOG;
         _pTeachBeginDialog->window()->setup();
         _pTeachBeginDialog->open();
      }
   }
}

bool EditWindow::prepareTeach(const IMove* newMove)
{
   IWiimoteHandler* pWHandler = g_pGame->getStageManager()->getWiimoteHandler();
   IMoveEditor* pEditor = g_pGame->getStageManager()->getMoveEditor();
   if (pWHandler == NULL || pEditor == NULL) { return false; }

   pWHandler->teachRollback();
   if (newMove == NULL || _pTeachMove == newMove) {
      if (_pTeachMove == NULL) { return false; }
   } else {
      MoveEditee me;
      me.pMove = newMove;
      pEditor->editeeSelect(me);
      _pTeachMove = newMove;
   }
   pWHandler->teachBegin(_pTeachMove);

   __int64 t0 = _pTeachMove->getBeginTime();
   if (30000000 < t0) {
      t0 -= 30000000;
   } else {
      t0 = 0;
   }

   EvSeekMovie ev;
   ev._origin = EvSeekMovie::SET;
   ev._offset = t0;
   ev._play = true;
   g_pFnd->getEventManager()->queue(&ev);
   return true;
}

bool EditWindow::onTeachBeginDialog(const CEGUI::EventArgs&)
{
   int i = _pTeachBeginDialog->window()->getDialogedButton();
   int next_stage = TEACH_NONE;

   if (i == TEACH_BEGIN_CANCEL) {
      next_stage = TEACH_NONE;
      
   } else {
      next_stage = TEACH_NONE;
      do {
         MoveEditee me;
         IMoveEditor* pEditor = g_pGame->getStageManager()->getMoveEditor();
         if (pEditor == NULL) { break; }
         if (! pEditor->editeeSelected(&me)) { break; }

         if (prepareTeach(me.pMove)) {
            next_stage = TEACH_PREPARE;
         }
      } while (0);
      /*
      next_stage = TEACH_NONE;
      do {
         MoveEditee me;
         IMoveEditor* pEditor = g_pGame->getStageManager()->getMoveEditor();
         IWiimoteHandler* pWHandler = g_pGame->getStageManager()->getWiimoteHandler();
         if (pEditor == NULL || pWHandler == NULL) { break; }
         if (! pEditor->editeeSelected(&me)) { break; }

         if (_pTeachMove) {
            pWHandler->teachRollback();
            _pTeachMove = NULL;
         }
         pWHandler->teachBegin(me.pMove);
         _pTeachMove = me.pMove;

         __int64 t0 = me.pMove->getBeginTime();
         if (30000000 < t0) {
            t0 -= 30000000;
         } else {
            t0 = 0;
         }

         EvSeekMovie ev;
         ev._origin = EvSeekMovie::SET;
         ev._offset = t0;
         ev._play = true;
         g_pFnd->getEventManager()->queue(&ev);
         next_stage = TEACH_TEACHING;
      } while (0);
      */
   }

   _teach_stage = next_stage;
   return true;
}

void EditWindow::onUpdate(const GameTime* gt)
{
   if (!_play) { return; }

   if (_teach_stage != TEACH_PREPARE && _teach_stage != TEACH_TEACHING) {
      return;
   }

   if (_teach_stage == TEACH_PREPARE) {
      ISceneSequencer* pSceneSeq = g_pGame->getStageManager()->getSceneSequencer();
      __int64 t = pSceneSeq->getScene(false).clock().clock;

      if (t < _pTeachMove->getBeginTime()) {
         _teach_stage = TEACH_TEACHING;
      }

   } else if (_teach_stage == TEACH_TEACHING) {
      ISceneSequencer* pSceneSeq = g_pGame->getStageManager()->getSceneSequencer();
      __int64 t = pSceneSeq->getScene(false).clock().clock;

      if (_pTeachMove->getEndTime() + 10000000 < t) {
         _pTeachResultDialog->window()->setup();
         _pTeachResultDialog->open();
         _teach_stage = TEACH_RESULTDIALOG;

         EvPauseMovie ev;
         g_pFnd->getEventManager()->queue(&ev);
      }
   }
}

bool EditWindow::onTeachResultDialog(const CEGUI::EventArgs&)
{
   IWiimoteHandler* pWHandler = g_pGame->getStageManager()->getWiimoteHandler();
   if (pWHandler == NULL) { return true; }

   int i = _pTeachResultDialog->window()->getDialogedButton();
   if (i == TEACH_RESULT_SUCCEED) {
      pWHandler->teachCommit(true);
   } else {
      pWHandler->teachCommit(false);
   }

   _pTeachContDialog->window()->setup();
   _pTeachContDialog->open();
   _teach_stage = TEACH_CONTDIALOG;
   return true;
}

bool EditWindow::onTeachContDialog(const CEGUI::EventArgs&)
{
   int i = _pTeachContDialog->window()->getDialogedButton();

   int next_stage = TEACH_NONE;
   if (i == TEACH_CONT_RETRY) {
      if (prepareTeach(NULL)) {
         next_stage = TEACH_PREPARE;
      }

   } else if (i == TEACH_CONT_NEXT) {
      do {
         IMoveEditor* pEditor = g_pGame->getStageManager()->getMoveEditor();
         if (pEditor == NULL) { break; }

         std::vector< const IMove* > moves;
         pEditor->getModels(&moves);
         size_t mi;
         for (mi=0; mi<moves.size(); ++mi) {
            if (moves[mi] == _pTeachMove) { break; }
         }
         ++mi;
         if (moves.size() <= mi) { break; }

         if (prepareTeach(moves[mi])) {
            next_stage = TEACH_PREPARE;
         }
      } while (0);
   }
   _teach_stage = next_stage;
   return true;

/*
   int next_stage;

   next_stage = TEACH_NONE;
   if (i == TEACH_CONT_RETRY || i == TEACH_CONT_NEXT) {
      do {
         IMoveEditor* pEditor = g_pGame->getStageManager()->getMoveEditor();
         IWiimoteHandler* pWHandler = g_pGame->getStageManager()->getWiimoteHandler();
         if (pEditor == NULL || pWHandler == NULL) { break; }

         std::vector< const IMove* > moves;
         pEditor->getModels(&moves);
         size_t mi;
         for (mi=0; mi<moves.size(); ++mi) {
            if (moves[mi] == _pTeachMove) { break; }
         }
         if (i == TEACH_CONT_NEXT) {
            ++mi;
         }
         if (moves.size() <= mi) { break; }

         {
            MoveEditee me;
            me.pMove = moves[mi];
            pEditor->editeeSelect(me);
         }
         _pTeachMove = moves[mi];
         pWHandler->teachBegin(_pTeachMove);

         __int64 t0 = _pTeachMove->getBeginTime();
         if (30000000 < t0) {
            t0 -= 30000000;
         } else {
            t0 = 0;
         }

         EvSeekMovie ev;
         ev._origin = EvSeekMovie::SET;
         ev._offset = t0;
         ev._play = true;
         g_pFnd->getEventManager()->queue(&ev);
         next_stage = TEACH_PREPARE;
      } while (0);
   }

   _teach_stage = next_stage;
   return true;
*/
}

//-------------------------------------------------------------
bool EditWindow::onMouseLeaves(const CEGUI::EventArgs& e_)
{
//   _pScreenMenu->hide();
   return true;
}

bool EditWindow::onMouseMove(const CEGUI::EventArgs& e_)
{
   if (_pen_main == EditPen::PEN_ACTION &&
      _pen_main == EditPen::ACTION_DEFAULT) {
         return onDefaultMouseMove(e_);
   }
   return false;
}
bool EditWindow::onMouseUp(const CEGUI::EventArgs& e_)
{
   if (_pen_main == EditPen::PEN_ACTION &&
      _pen_main == EditPen::ACTION_DEFAULT) {
         return onDefaultMouseUp(e_);
   }
   return false;
}
bool EditWindow::onMouseDown(const CEGUI::EventArgs& e_)
{
   _pScreenMenu->hide();

   const CEGUI::MouseEventArgs& e = static_cast< const CEGUI::MouseEventArgs& >(e_);
   if (e.button == CEGUI::LeftButton) {
      switch (_pen_main) {
      case EditPen::PEN_ACTION:
         if (_pen_main == EditPen::ACTION_DEFAULT) {
            return onDefaultMouseDown(e_);
         }
         break;
      case EditPen::PEN_NEWMOVE:
         return onNewMoveMouseDown(e_);
      }

   } else if (e.button == CEGUI::RightButton) {
      return onDefaultMouseRightDown(e_);
   }
   return false;
}

bool EditWindow::onDragDrop(const CEGUI::EventArgs& e_)
{
   const CEGUI::DragDropEventArgs& e = static_cast< const CEGUI::DragDropEventArgs& >(e_);
   return true;
}
bool EditWindow::onDragEnter(const CEGUI::EventArgs& e_)
{
   const CEGUI::DragDropEventArgs& e = static_cast< const CEGUI::DragDropEventArgs& >(e_);
   return true;
}
bool EditWindow::onDragLeave(const CEGUI::EventArgs& e_)
{
   const CEGUI::DragDropEventArgs& e = static_cast< const CEGUI::DragDropEventArgs& >(e_);
   return true;
}

bool EditWindow::init(CeguiTextureImage* pImage)
{
   //   if (! _edit.init(pProv)) { return false; }
   if (! pImage->addLook(&_image_id)) {
      return false;
   }

   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();
   {
      CEGUI::UDim x(1,0), y(1,0);
      _pFrame = wm.createWindow("DefaultWindow", (CEGUI::utf8*)"Frame");
      _pFrame->setSize(CEGUI::UVector2(x,y));

      CEGUI::WindowFactoryManager::addFactory<
         CEGUI::TplWindowFactory< PenWindow > >();
      _pPenWin = static_cast< PenWindow* >(wm.createWindow(PenWindow::WidgetTypeName, "PenWin"));
      _pPenWin->init();
      this->addChildWindow(_pPenWin);

      CEGUI::WindowFactoryManager::addFactory<
         CEGUI::TplWindowFactory< ValueWindow > >();
      _pValueWin = static_cast< ValueWindow* >(wm.createWindow(ValueWindow::WidgetTypeName, "ValueWin"));
      _pValueWin->init();
      this->addChildWindow(_pValueWin);

      _pWiimoteWin = NULL;
/*
      CEGUI::WindowFactoryManager::addFactory<
         CEGUI::TplWindowFactory< WiimoteWindow > >();
      _pWiimoteWin = static_cast< WiimoteWindow* >(wm.createWindow(WiimoteWindow::WidgetTypeName, "WiimoteWin"));
      _pWiimoteWin->init();
      this->addChildWindow(_pWiimoteWin);
*/
      _pScreen = wm.createWindow("TaharezLook/StaticImage", "Screen");
      _pScreen->setSize(CEGUI::UVector2(x,y));
      _pScreen->setPosition(CEGUI::UVector2(x,y));
      //_pScreen->setUsingAutoRenderingSurface(false);
      _pScreen->setUsingAutoRenderingSurface(true);

#define SUBSCRIBE(EVENT, METHOD)\
      _pScreen->subscribeEvent(\
         CEGUI::DefaultWindow:: EVENT,\
         CEGUI::SubscriberSlot(\
           &EditWindow:: METHOD, this));
      SUBSCRIBE(EventMouseMove, onMouseMove);
      SUBSCRIBE(EventMouseButtonDown, onMouseDown);
      SUBSCRIBE(EventMouseButtonUp, onMouseUp);
      SUBSCRIBE(EventDragDropItemDropped, onDragDrop);
      SUBSCRIBE(EventDragDropItemEnters, onDragEnter);
      SUBSCRIBE(EventDragDropItemLeaves, onDragLeave);
      SUBSCRIBE(EventMouseLeaves, onMouseLeaves);
#undef SUBSCRIBE

      _pScreenMenu = static_cast< CEGUI::PopupMenu* >(wm.loadWindowLayout("ScreenContextMenu.layout"));
      _pScreenMenu->setFont(::bootes::cegui::DynamicFont::GetAsciiFont(4));
      _pScreen->addChildWindow(_pScreenMenu);

#define SUBSCRIBE_MENU(NAME)                                            \
      wm.getWindow("Screen/ContextMenu/" #NAME)->subscribeEvent(\
         CEGUI::MenuItem::EventClicked,\
         CEGUI::SubscriberSlot(&EditWindow::onMenu ##NAME, this));

      SUBSCRIBE_MENU(Copy);
      SUBSCRIBE_MENU(Cut);
      SUBSCRIBE_MENU(Paste);
      SUBSCRIBE_MENU(Delete);
      SUBSCRIBE_MENU(InsertBefore);
      SUBSCRIBE_MENU(InsertAfter);
      SUBSCRIBE_MENU(DeletePoint);
      SUBSCRIBE_MENU(TeachReset);
#undef SUBSCRIBE_MENU

      _pFrame->addChildWindow(_pScreen);
      this->addChildWindow(_pFrame);
   }

   {
      _pCtrl = static_cast< CEGUI::HorizontalLayoutContainer* >(wm.createWindow("HorizontalLayoutContainer", "Ctrl"));

      CEGUI::PushButton* pBtn;
#define CREATE_BUTTON(NAME,VAL) {\
  pBtn = static_cast< CEGUI::PushButton* >(wm.createWindow("TaharezLook/Button", NAME));\
  pBtn->setText(VAL);\
}
#define REG_CLICK(METHOD) {\
  pBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::SubscriberSlot(&EditWindow:: METHOD, this));\
}
      CEGUI::UDim d(0, CONTROL_BUTTON_SIZE);
      CREATE_BUTTON("PlayButton", ">");
      REG_CLICK(doPlay);
      _pCtrlBtn[CTRL_PLAY] = pBtn;
      _pCtrlBtn[CTRL_PLAY]->setSize(CEGUI::UVector2(d,d));
      _pCtrlBtn[CTRL_PLAY]->setEnabled(false);
      _pCtrlBtn[CTRL_PLAY]->setFont(::bootes::cegui::DynamicFont::GetAsciiFont(4));
      _pCtrl->addChildWindow(_pCtrlBtn[CTRL_PLAY]);

      CREATE_BUTTON("Back1Button", "<<");
      REG_CLICK(doBack1);
      _pCtrlBtn[CTRL_BACK1] = pBtn;
      _pCtrlBtn[CTRL_BACK1]->setSize(CEGUI::UVector2(d,d));
      _pCtrlBtn[CTRL_BACK1]->setEnabled(false);
      _pCtrlBtn[CTRL_BACK1]->setFont(::bootes::cegui::DynamicFont::GetAsciiFont(4));
      _pCtrl->addChildWindow(_pCtrlBtn[CTRL_BACK1]);

      CREATE_BUTTON("Foward1Button", ">>");
      REG_CLICK(doForward1);
      _pCtrlBtn[CTRL_FORWARD1] = pBtn;
      _pCtrlBtn[CTRL_FORWARD1]->setSize(CEGUI::UVector2(d,d));
      _pCtrlBtn[CTRL_FORWARD1]->setEnabled(false);
      _pCtrlBtn[CTRL_FORWARD1]->setFont(::bootes::cegui::DynamicFont::GetAsciiFont(4));
      _pCtrl->addChildWindow(_pCtrlBtn[CTRL_FORWARD1]);

#undef CREATE_BUTTON
#undef REG_CLICK

//      _pCtrl->addChildWindow(_pCtrlBtn[CTRL_BACK]);
//      _pCtrl->addChildWindow(_pCtrlBtn[CTRL_FORWARD]);
      this->addChildWindow(_pCtrl);
   }
   {
/*
      _pSlider = static_cast< CEGUI::Slider* >(wm.createWindow("TaharezLook/Slider", "Slider"));
      this->addChildWindow(_pSlider);
*/
      _pFrameBar = static_cast< CEGUI::Scrollbar* >(wm.createWindow("TaharezLook/HorizontalScrollbar", "FrameBar"));
      _pFrameBar->subscribeEvent(
         CEGUI::Scrollbar::EventScrollPositionChanged,
         CEGUI::SubscriberSlot(&EditWindow::doFrame, this));
      _pFrameBar->subscribeEvent(
         CEGUI::Scrollbar::EventThumbTrackStarted,
         CEGUI::SubscriberSlot(&EditWindow::doSuspend, this));
      _pFrameBar->subscribeEvent(
         CEGUI::Scrollbar::EventThumbTrackEnded,
         CEGUI::SubscriberSlot(&EditWindow::doResume, this));
      _pFrameBar->getDecreaseButton()->setVisible(false);
      _pFrameBar->getIncreaseButton()->setVisible(false);
      this->addChildWindow(_pFrameBar);

      _pFrameText = static_cast< CEGUI::DefaultWindow* >(wm.createWindow("TaharezLook/StaticText","FrameText"));
      _pFrameText->setFont(::bootes::cegui::DynamicFont::GetAsciiFont(4));
      _pFrameText->setProperty("FrameEnabled", "false");
      this->addChildWindow(_pFrameText);

      _pTimeText = static_cast< CEGUI::DefaultWindow* >(wm.createWindow("TaharezLook/StaticText","TimeText"));
      _pTimeText->setFont(::bootes::cegui::DynamicFont::GetAsciiFont(4));
      _pTimeText->setProperty("FrameEnabled", "false");
      this->addChildWindow(_pTimeText);
   }

   {
      CEGUI::WindowFactoryManager::addFactory<
         CEGUI::TplWindowFactory< TimeEditWindow > >();
      _pTimeEdit = static_cast< TimeEditWindow* >(wm.createWindow(TimeEditWindow::WidgetTypeName, "TimeEdit"));
      _pTimeEdit->init();
      this->addChildWindow(_pTimeEdit);
   }

   {
      int w = 0;
      w = ::std::max(w, (int)(SCREEN_MIN_WIDTH + SIDE_WIDTH*2));
      w = ::std::max(w, (int)(CONTROL_MIN_WIDTH));

      int h = 0;
      h  = SCREEN_MIN_HEIGHT + PADDING_HEIGHT * 2;
      h += CONTROL_HEIGHT    + PADDING_HEIGHT * 2;
      h += TIMEEDIT_HEIGHT   + PADDING_HEIGHT * 2;

      CEGUI::UDim uw(0.0f, (float)w);
      CEGUI::UDim uh(0.0f, (float)h);
      setMinSize(CEGUI::UVector2(uw,uh));
   }

   {
      _pTeachBeginDialog = ::bootes::cegui::MessageDialog::Create("");
      _pTeachBeginDialog->window()->setMessage("start teach?");
      _pTeachBeginDialog->window()->addButton(TEACH_BEGIN_START, "A: Start", ::bootes::lib::framework::WiimoteEvent::BTN_A);
      _pTeachBeginDialog->window()->addButton(TEACH_BEGIN_CANCEL, "B: Cancel", ::bootes::lib::framework::WiimoteEvent::BTN_B);
      _pTeachBeginDialog->subscribeEvent(
         ::bootes::cegui::WindowDialog::EventSubmit,
         ::CEGUI::SubscriberSlot(&EditWindow::onTeachBeginDialog, this));
      this->addChildWindow(_pTeachBeginDialog);
      _pTeachBeginDialog->hide();

      _pTeachResultDialog = ::bootes::cegui::MessageDialog::Create("");
      _pTeachResultDialog->window()->setMessage("succeed to move?");
      _pTeachResultDialog->window()->addButton(TEACH_RESULT_SUCCEED, "A: Succeed", ::bootes::lib::framework::WiimoteEvent::BTN_A);
      _pTeachResultDialog->window()->addButton(TEACH_RESULT_FAILED, "B: Failed", ::bootes::lib::framework::WiimoteEvent::BTN_B);
      _pTeachResultDialog->subscribeEvent(
         ::bootes::cegui::WindowDialog::EventSubmit,
         ::CEGUI::SubscriberSlot(&EditWindow::onTeachResultDialog, this));
      this->addChildWindow(_pTeachResultDialog);
      _pTeachResultDialog->hide();

      _pTeachContDialog = ::bootes::cegui::MessageDialog::Create("");
      _pTeachContDialog->window()->setMessage("continue teaching?");
      _pTeachContDialog->window()->addButton(TEACH_CONT_RETRY, "A: Retry.", ::bootes::lib::framework::WiimoteEvent::BTN_A);
      _pTeachContDialog->window()->addButton(TEACH_CONT_NEXT, "+: Next.", ::bootes::lib::framework::WiimoteEvent::BTN_PLUS);
      _pTeachContDialog->window()->addButton(TEACH_CONT_FINISH, "B: Finish.", ::bootes::lib::framework::WiimoteEvent::BTN_B);
      _pTeachContDialog->subscribeEvent(
         ::bootes::cegui::WindowDialog::EventSubmit,
         ::CEGUI::SubscriberSlot(&EditWindow::onTeachContDialog, this));
      this->addChildWindow(_pTeachContDialog);
      _pTeachContDialog->hide();
   }

   _pImage = pImage;


   return true;
}

void EditWindow::layout()
{
   CEGUI::Rect rect = getUnclippedInnerRect();
   CEGUI::Size size;
   size.d_width  = rect.d_right - rect.d_left;
   size.d_height = rect.d_bottom - rect.d_top;
   //getPixelSize() は装飾領域も含んだサイズになるみたい
   //CEGUI::Size size = getPixelSize();

   // 子ウィンドウのサイズと位置の計算。
   {
      CEGUI::UDim h(0,0), w(0,0), x(0,0), y(0,0);
      int y0 = 0;

      { //上部。中央に動画表示画面、左に操作ペン、右に値表示。高さは可変。
         int xpad = 10;
         int h0 = size.d_height
            - (CONTROL_HEIGHT  + PADDING_HEIGHT*2) - (TIMEEDIT_HEIGHT + PADDING_HEIGHT*2);
         x.d_offset = xpad;
         y.d_offset = y0 + PADDING_HEIGHT;
         w.d_offset = SIDE_WIDTH;
         h.d_offset = h0 - (PADDING_HEIGHT*2);

         _pPenWin->setSize(CEGUI::UVector2(w,h));
         _pPenWin->setPosition(CEGUI::UVector2(x,y));

         x.d_offset += w.d_offset + xpad;
         w.d_offset = size.d_width - (2*SIDE_WIDTH) - xpad*4; //左右端(=2)と、3つの画面の間(=2)各10ピクセル
         _pFrame->setSize(CEGUI::UVector2(w,h));
         _pFrame->setPosition(CEGUI::UVector2(x,y));

         x.d_offset += w.d_offset + xpad;
         w.d_offset = SIDE_WIDTH;
         h.d_offset = h0 / 2;
         _pValueWin->setSize(CEGUI::UVector2(w,h));
         _pValueWin->setPosition(CEGUI::UVector2(x,y));
/*
         y.d_offset += h.d_offset;
         _pWiimoteWin->setSize(CEGUI::UVector2(w,h));
         _pWiimoteWin->setPosition(CEGUI::UVector2(x,y));
*/
         y0 += h0;
      }

      {  // スクリーンの下にコントロールバーをとる
         // ボタン他の固定サイズを確定し、余りをスライダーに
         int xpad = 10;
         x.d_offset = xpad;
         y.d_offset = y0 + PADDING_HEIGHT;
         w.d_offset = CONTROL_BUTTON_WIDTH - xpad*2;
         h.d_offset = CONTROL_HEIGHT - PADDING_HEIGHT*2;
         _pCtrl->setSize(CEGUI::UVector2(w,h));
         _pCtrl->setPosition(CEGUI::UVector2(x,y));

         x.d_offset += w.d_offset + xpad;
         w.d_offset = size.d_width
            - CONTROL_BUTTON_WIDTH 
            - CONTROL_FRAMETEXT_WIDTH
            - (2 * CONTROL_BUTTON_SIZE)
            - 4*xpad;
         _pFrameBar->setSize(CEGUI::UVector2(w,h));
         _pFrameBar->setPosition(CEGUI::UVector2(x,y));
/*
ボタンサイズ変えられないぽい
         x.d_offset = CONTROL_BUTTON_SIZE;
         _pFrameBar->getDecreaseButton()->setSize(CEGUI::UVector2(x,y));
         _pFrameBar->getIncreaseButton()->setSize(CEGUI::UVector2(x,y));
*/
         x.d_offset += w.d_offset + xpad;
         y.d_offset = y0 + PADDING_HEIGHT;
         w.d_offset = CONTROL_FRAMETEXT_WIDTH;
         h.d_offset = CONTROL_FRAMETEXT_HEIGHT;
         _pFrameText->setSize(CEGUI::UVector2(w,h));
         _pFrameText->setPosition(CEGUI::UVector2(x,y));

         y.d_offset += h.d_offset;
         _pTimeText->setSize(CEGUI::UVector2(w,h));
         _pTimeText->setPosition(CEGUI::UVector2(x,y));

         y0 += CONTROL_HEIGHT + PADDING_HEIGHT*2;
      }
      {
         y0 = size.d_height - (TIMEEDIT_HEIGHT + PADDING_HEIGHT*2);
         x.d_offset = 10;
         y.d_offset = y0 + PADDING_HEIGHT;
         w.d_offset = size.d_width - 20;
         h.d_offset = TIMEEDIT_HEIGHT;
         _pTimeEdit->setSize(CEGUI::UVector2(w,h));
         _pTimeEdit->setPosition(CEGUI::UVector2(x,y));
      }
   }
}

void EditWindow::onRender(const GameTime* gt)
{
   {
      const CEGUI::Size& s0 = _pImage->getSize();
      CEGUI::Size s1 = _pFrame->getPixelSize();
      CEGUI::UDim x(0,0), y(0,0), w(0,0), h(0,0);

      float rx = s1.d_width / s0.d_width;
      float ry = s1.d_height / s0.d_height;
      if (rx < ry) {
         w.d_offset = s0.d_width  * rx;
         h.d_offset = s0.d_height * rx;
      } else {
         w.d_offset = s0.d_width  * ry;
         h.d_offset = s0.d_height * ry;
      }
      x.d_offset = (s1.d_width  - w.d_offset) / 2;
      y.d_offset = (s1.d_height - h.d_offset) / 2;

      _pScreen->setSize(CEGUI::UVector2(w,h));
      _pScreen->setPosition(CEGUI::UVector2(x,y));
   }
   {
      __int64 t = _pImage->getTime();
      setFrameText(_pFrameText, _pTimeText, _videoInfo.size_100ns, t, _videoInfo.tpf_100ns);
      _pFrameBar->setScrollPosition(t + 0.0f);
      _pImage->renderImage(_image_id, static_cast< CEGUI::DefaultWindow* >(_pScreen));
   }
   //_pWiimoteWin->onRender(currentTime, elapsedTime);
}

namespace {
inline bool _mouse_pos(const CEGUI::Window* win, const CEGUI::MouseEventArgs& e, float* x, float* y, float* rx, float *ry, float *w, float* h)
{
   CEGUI::Size size = win->getPixelSize();
   if (size.d_width < 1 || size.d_height < 1) {
      return false;
   }
   CEGUI::Vector2 pos = 
      CEGUI::CoordConverter::screenToWindow(*win, e.position);
   *x  = pos.d_x;
   *y  = pos.d_y;
   *w = size.d_width;
   *h = size.d_height;
   *rx = pos.d_x / size.d_width;
   *ry = pos.d_y / size.d_height;
   return true;
}
}

bool EditWindow::onDefaultMouseMove(const CEGUI::EventArgs& e_)
{
   const CEGUI::MouseEventArgs& e = static_cast< const CEGUI::MouseEventArgs& >(e_);
   float x,y,rx,ry,w,h;
   if (! _mouse_pos(_pScreen, e, &x, &y, &rx, &ry, &w, &h)) {
      return true;
   }

   IMove* pMove;
   MoveEditee editee;
   IMoveEditor* pEditor = g_pGame->getStageManager()->getMoveEditor();
   if (pEditor == NULL) { return true; }
   if (! pEditor->editing(&editee, &pMove)) { return true; }
   IGuide* pGuide = pMove->getGuide();
   if (pGuide == NULL) { return true; }

   if (editee.joint < 0) {
      pGuide->translate(rx - _rx0, ry - _ry0);
   } else {
      pGuide->replace(editee.joint, D3DXVECTOR3(rx, ry, 0));
   }
   _rx0 = rx;
   _ry0 = ry;

   return true;
}

bool EditWindow::onDefaultMouseDown(const CEGUI::EventArgs& e_)
{
   const CEGUI::MouseEventArgs& e = static_cast< const CEGUI::MouseEventArgs& >(e_);
   float x,y,rx,ry,w,h;
   if (! _mouse_pos(_pScreen, e, &x, &y, &rx, &ry, &w, &h)) {
      return true;
   }

   IMoveEditor* pEditor  = g_pGame->getStageManager()->getMoveEditor();
   IMovePresenter* pPres = g_pGame->getStageManager()->getMovePresenter();
   if (pEditor == NULL || pPres == NULL) { return true; }

   if (e.button == CEGUI::LeftButton && e.clickCount == 1) {
      IMove* pMove;
      MoveEditee editee;
      if (pEditor->editing(&editee, &pMove)) { return true; }
      if (! pPres->presentLocated(rx, ry, &editee)) { return true; }
      if (! pEditor->editeeSelect(editee)) { return true; }
      if (! pEditor->editBegin()) { return true; }
      _rx0 = rx;
      _ry0 = ry;
   }
   return true;
}

bool EditWindow::onDefaultMouseUp(const CEGUI::EventArgs& e_)
{
   const CEGUI::MouseEventArgs& e = static_cast< const CEGUI::MouseEventArgs& >(e_);
   float x,y,rx,ry,w,h;
   if (! _mouse_pos(_pScreen, e, &x, &y, &rx, &ry, &w, &h)) {
      return true;
   }

   IMoveEditor* pEditor  = g_pGame->getStageManager()->getMoveEditor();
   if (pEditor == NULL) { return true; }
   if (e.button == CEGUI::LeftButton && e.clickCount == 1) {
      IMove* pMove;
      MoveEditee editee;
      if (! pEditor->editing(&editee, &pMove)) { return true; }
      if (pMove->getGuide()) {
         pMove->getGuide()->normalizeEditPoints();
      }
      pEditor->editCommit();
   }
   return true;
}

bool EditWindow::onNewMoveMouseDown(const CEGUI::EventArgs& e_)
{
   const CEGUI::MouseEventArgs& e = static_cast< const CEGUI::MouseEventArgs& >(e_);
   float x,y,rx,ry,w,h;
   if (! _mouse_pos(_pScreen, e, &x, &y, &rx, &ry, &w, &h)) {
      return true;
   }

   IGuide* pGuide = NULL;
   switch (_pen_sub) {
   case EditPen::MOVE_LINE:
      {
         GuideRibbonLine* p = new GuideRibbonLine();
         if (1.0f < rx+0.1f) {
            p->init(rx-0.1f, ry, rx, ry);
         } else {
            p->init(rx, ry, rx+0.1f, ry);
         }
         pGuide = p;
      }
      break;
   case EditPen::MOVE_ELLIPSE:
      {
         GuideRibbonEllipse* p = new GuideRibbonEllipse();
         p->init(rx, ry, 0.3f, 0.3f, -FLOAT_PI*0.4f, -FLOAT_PI*0.6f, true);
         pGuide = p;
      }
      break;
   case EditPen::MOVE_SPLINE:
      {
         GuideRibbonSpline* p = new GuideRibbonSpline();
         pGuide = p;
      }
      break;
   default:
      break;
   }
   if (pGuide == NULL) { return false; }

   IMove* pMove = g_pGame->getStageManager()->createMove(pGuide);

   ISceneSequencer* pSceneSeq = g_pGame->getStageManager()->getSceneSequencer();
   IMoveEditor*     pEditor   = g_pGame->getStageManager()->getMoveEditor();
   if (pMove != NULL && pSceneSeq != NULL && pEditor != NULL) {
      __int64 t0 = pSceneSeq->getScene(false).clock().clock;
      __int64 t1 = t0 + 10*1000*1000;
      if (! pEditor->addModel(pMove, t0, t1)) {
         delete pMove;
         return false;
      }
   }
   {
      EvEditorPen ev;
      ev._main = EditPen::PEN_ACTION;
      ev._sub  = EditPen::ACTION_DEFAULT;
      g_pFnd->getEventManager()->queue(&ev);
   }
   return true;
}

bool EditWindow::onDefaultMouseRightDown(const CEGUI::EventArgs& e_)
{
   const CEGUI::MouseEventArgs& e = static_cast< const CEGUI::MouseEventArgs& >(e_);
   if (e.clickCount != 1) { return true; }

   float x,y,rx,ry,w,h;
   if (! _mouse_pos(_pScreen, e, &x, &y, &rx, &ry, &w, &h)) {
      return true;
   }

   IMoveEditor* pEditor  = g_pGame->getStageManager()->getMoveEditor();
   IMovePresenter* pPres = g_pGame->getStageManager()->getMovePresenter();
   if (pEditor == NULL || pPres == NULL) { return true; }

   CEGUI::PopupMenu* pMenu = NULL;
   do {
      CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();
      MoveEditee editee;
      if ( !pPres->presentLocated(rx, ry, &editee) ) {
         wm.getWindow("Screen/ContextMenu/Copy")->setEnabled(false);
         wm.getWindow("Screen/ContextMenu/Cut")->setEnabled(false);
         wm.getWindow("Screen/ContextMenu/Paste")->setEnabled( (_copied_move != NULL) );
         wm.getWindow("Screen/ContextMenu/Delete")->setEnabled(false);
         wm.getWindow("Screen/ContextMenu/InsertBefore")->setEnabled(false);
         wm.getWindow("Screen/ContextMenu/InsertAfter")->setEnabled(false);
         wm.getWindow("Screen/ContextMenu/DeletePoint")->setEnabled(false);
         wm.getWindow("Screen/ContextMenu/TeachReset")->setEnabled(false);
         pMenu = _pScreenMenu;

      } else if (editee.joint < 0) {
         if (! pEditor->editeeSelect(editee)) { break; }
         wm.getWindow("Screen/ContextMenu/Copy")->setEnabled(true);
         wm.getWindow("Screen/ContextMenu/Cut")->setEnabled(true);
         wm.getWindow("Screen/ContextMenu/Paste")->setEnabled(false);
         wm.getWindow("Screen/ContextMenu/Delete")->setEnabled(true);
         wm.getWindow("Screen/ContextMenu/InsertBefore")->setEnabled(false);
         wm.getWindow("Screen/ContextMenu/InsertAfter")->setEnabled(false);
         wm.getWindow("Screen/ContextMenu/DeletePoint")->setEnabled(false);
         wm.getWindow("Screen/ContextMenu/TeachReset")->setEnabled(true);
         pMenu = _pScreenMenu;

      } else {
         if (! pEditor->editeeSelect(editee)) { break; }
         wm.getWindow("Screen/ContextMenu/Copy")->setEnabled(true);
         wm.getWindow("Screen/ContextMenu/Cut")->setEnabled(true);
         wm.getWindow("Screen/ContextMenu/Paste")->setEnabled(false);
         wm.getWindow("Screen/ContextMenu/Delete")->setEnabled(true);
         wm.getWindow("Screen/ContextMenu/TeachReset")->setEnabled(true);

         const IGuide* pGuide = editee.pMove->getGuide();
         bool b;
         b = (pGuide)? pGuide->canInsert(editee.joint): false;
         wm.getWindow("Screen/ContextMenu/InsertBefore")->setEnabled(b);

         b = (pGuide)? pGuide->canInsert(editee.joint+1): false;
         wm.getWindow("Screen/ContextMenu/InsertAfter")->setEnabled(b);

         b = (pGuide)? pGuide->canErase(editee.joint): false;
         wm.getWindow("Screen/ContextMenu/DeletePoint")->setEnabled(b);
         pMenu = _pScreenMenu;

      }
   } while (0);

   if (pMenu != _pScreenMenu) {
      _pScreenMenu->hide();
   }
   if (pMenu != NULL) {
      CEGUI::Size size0 = this->getPixelSize();
      CEGUI::Size size1 = pMenu->getPixelSize();
      if (size0.d_width  < x + size1.d_width) { x = size0.d_width - size1.d_width; }
      if (size0.d_height < y + size1.d_height) { y = size0.d_height - size1.d_height; }
      
      CEGUI::UVector2 pos(CEGUI::UDim(0,x), CEGUI::UDim(0,y));
      pMenu->setPosition(pos);
      pMenu->show();
   }
   _rx0 = rx;
   _ry0 = ry;

   return true;
}

bool EditWindow::onMenuCopy(const CEGUI::EventArgs& ev)
{
   return onMenuCopyCutDelete(ev, true, false);
}
bool EditWindow::onMenuCut(const CEGUI::EventArgs& ev)
{
   return onMenuCopyCutDelete(ev, true, true);
}
bool EditWindow::onMenuDelete(const CEGUI::EventArgs& ev)
{
   return onMenuCopyCutDelete(ev, false, true);
}
bool EditWindow::onMenuCopyCutDelete(const CEGUI::EventArgs& e_, bool copy, bool del)
{
   IMoveEditor* pEditor  = g_pGame->getStageManager()->getMoveEditor();
   if (pEditor == NULL) { return true; }

   MoveEditee editee;
   if (! pEditor->editeeSelected(&editee)) { return true; }

   if (copy) {
      if (_copied_move) { delete _copied_move; }
      _copied_move = editee.pMove->clone();
   }
   if (del) {
      pEditor->editeeDeleteSelected();
   }
   return true;
}
bool EditWindow::onMenuPaste(const CEGUI::EventArgs& ev)
{
   if (_copied_move == NULL) { return true; }

   IMoveEditor* pEditor  = g_pGame->getStageManager()->getMoveEditor();
   ISceneSequencer* pSceneSeq = g_pGame->getStageManager()->getSceneSequencer();
   if (pEditor == NULL || pSceneSeq == NULL) { return true; }

   __int64 t0 = pSceneSeq->getScene(false).clock().clock;
   __int64 t1 = t0 + (_copied_move->getEndTime() - _copied_move->getBeginTime());

   IMove* p = _copied_move->clone();
   if (! pEditor->addModel(p, t0, t1)) {
      delete p;
   }
   return true;
}


bool EditWindow::onMenuInsertBefore(const CEGUI::EventArgs& e_) { return onMenuInsert(e_, true); }
bool EditWindow::onMenuInsertAfter(const CEGUI::EventArgs& e_) { return onMenuInsert(e_, false); }

bool EditWindow::onMenuInsert(const CEGUI::EventArgs& e_, bool before)
{
   IMoveEditor* pEditor  = g_pGame->getStageManager()->getMoveEditor();
   if (pEditor == NULL) { return true; }
   if (! pEditor->editBegin()) { return true; }

   IMove *pMove;
   IGuide* pGuide;
   MoveEditee editee;
   if (! pEditor->editing(&editee, &pMove)) { return true; }
   pGuide = pMove->getGuide();
   if (pGuide == NULL) { return true; }
   if (editee.joint < 0 || pGuide->getEditPoints().size() <= editee.joint) { return true; }

   D3DXVECTOR3 pos(_rx0, _ry0, 0.0f);
   size_t idx = (before)? editee.joint: editee.joint + 1;
   if (! pGuide->insert(idx, pos)) {
      pEditor->editRollback();
   } else {
      pEditor->editCommit();
   }
   return true;
}

bool EditWindow::onMenuDeletePoint(const CEGUI::EventArgs& e_)
{
   IMoveEditor* pEditor  = g_pGame->getStageManager()->getMoveEditor();
   if (pEditor == NULL) { return true; }
   if (! pEditor->editBegin()) { return true; }

   IMove *pMove;
   IGuide* pGuide;
   MoveEditee editee;
   if (! pEditor->editing(&editee, &pMove)) { return true; }
   pGuide = pMove->getGuide();
   if (pGuide == NULL) { return true; }

   size_t idx = editee.joint;
   if (! pGuide->erase(idx)) {
      pEditor->editRollback();
   } else {
      pEditor->editCommit();
   }
   return true;
}

bool EditWindow::onMenuTeachReset(const CEGUI::EventArgs& ev)
{
   IMoveEditor* pEditor       = g_pGame->getStageManager()->getMoveEditor();
   IWiimoteHandler *pWHandler = g_pGame->getStageManager()->getWiimoteHandler();
   if (pEditor == NULL || pWHandler == NULL) { return true; }

   MoveEditee e;
   if (! pEditor->editeeSelected(&e)) { return true; }

   pWHandler->teachClear(e.pMove);
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
