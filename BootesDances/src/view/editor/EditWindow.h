#ifndef BOOTESDANCES_VIEW_CEGUI_CEGUIEDITWINDOW_H
#define BOOTESDANCES_VIEW_CEGUI_CEGUIEDITWINDOW_H

#pragma warning(disable : 4819)
#include "../../include.h"
#include <bootes/dances/EvMovie.h>
#include <bootes/dances/EvStage.h>
#include <bootes/dances/EvEditor.h>
#include <bootes/dances/IStageManager.h>
#include <bootes/cegui/MessageDialog.h>
#include "EditorDefines.h"
#include "CeguiTextureImage.h"
#include "TimeEditWindow.h"
#include "PenWindow.h"
#include "ValueWindow.h"
#include "WiimoteWindow.h"
#include <map>
#include <string>

#pragma warning(disable : 4995)
#include <CEGUI.h>

namespace bootes { namespace dances { namespace editor {

class EditWindow
   : public CEGUI::LayoutContainer
   , public ::bootes::lib::framework::EventListener
{
   typedef ::bootes::lib::framework::GameTime   GameTime;
   typedef ::bootes::lib::framework::InputEvent InputEvent;
public:
   static const CEGUI::String WidgetTypeName;

   EditWindow(const CEGUI::String& type, const CEGUI::String& name);
   ~EditWindow();
   bool init(CeguiTextureImage*);

   void onEvent(const GameTime* gt, const ::bootes::lib::framework::Event* ev);

   virtual void layout();
   void onUpdate(const GameTime* gt);
   void onRender(const GameTime* gt);
   void onInput(const GameTime* gt, const InputEvent* ev);

   bool doPlay(const CEGUI::EventArgs&);
   bool doPause(const CEGUI::EventArgs&);
   bool doSuspend(const CEGUI::EventArgs&);
   bool doResume(const CEGUI::EventArgs&);

   bool doBack(const CEGUI::EventArgs&);
   bool doForward(const CEGUI::EventArgs&);
   bool doBack1(const CEGUI::EventArgs&);
   bool doForward1(const CEGUI::EventArgs&);
   bool doFrame(const CEGUI::EventArgs&);

   bool onMouseLeaves(const CEGUI::EventArgs&);
   bool onMouseMove(const CEGUI::EventArgs&);
   bool onMouseUp(const CEGUI::EventArgs&);
   bool onMouseDown(const CEGUI::EventArgs&);
   bool onMouseClick(const CEGUI::EventArgs&);

   bool onDragEnter(const CEGUI::EventArgs&);
   bool onDragDrop(const CEGUI::EventArgs&);
   bool onDragLeave(const CEGUI::EventArgs&);

   void onWiimote(const ::bootes::lib::framework::WiimoteEvent*);
   bool onTeachBeginDialog(const CEGUI::EventArgs&);
   bool onTeachResultDialog(const CEGUI::EventArgs&);
   bool onTeachContDialog(const CEGUI::EventArgs&);

private:
   bool prepareTeach(const IMove*);

   void onLoad(bool result, const VideoInfo&);
   void onPlay(const EvMoviePlay*);
   void onPause(const EvMoviePause*);

   void onPen(const EvEditorPen*);
   bool onDefaultMouseUp(const CEGUI::EventArgs&);
   bool onDefaultMouseDown(const CEGUI::EventArgs&);
   bool onDefaultMouseMove(const CEGUI::EventArgs&);
   bool onNewMoveMouseDown(const CEGUI::EventArgs&);
   bool onDefaultMouseRightDown(const CEGUI::EventArgs&);

   bool onMenuCopy(const CEGUI::EventArgs&);
   bool onMenuCut(const CEGUI::EventArgs&);
   bool onMenuPaste(const CEGUI::EventArgs&);
   bool onMenuDelete(const CEGUI::EventArgs&);
   bool onMenuCopyCutDelete(const CEGUI::EventArgs&, bool copy, bool del);

   bool onMenuInsertBefore(const CEGUI::EventArgs&);
   bool onMenuInsertAfter(const CEGUI::EventArgs&);
   bool onMenuDeletePoint(const CEGUI::EventArgs&);
   bool onMenuInsert(const CEGUI::EventArgs&, bool before);
   bool onMenuTeach(const CEGUI::EventArgs&);
   bool onMenuTeachReset(const CEGUI::EventArgs&);

private:
   CeguiTextureImage* _pImage;
   std::string _image_id;
   VideoInfo _videoInfo;
   int _pen_main, _pen_sub;
   bool _isThumbTrack;
   float _rx0, _ry0;
   MoveEditee _menu_editee;
   IMove* _copied_move;

   CEGUI::Window*    _pFrame;
   PenWindow*   _pPenWin;
   ValueWindow* _pValueWin;
   WiimoteWindow* _pWiimoteWin;
   CEGUI::Window*    _pScreen;
   CEGUI::PopupMenu* _pScreenMenu;
   CEGUI::DefaultWindow *_pFrameText, *_pTimeText;
   CEGUI::HorizontalLayoutContainer* _pCtrl;
   CEGUI::Scrollbar* _pFrameBar;
   TimeEditWindow* _pTimeEdit;
   bool _play;

   enum {
      CTRL_PLAY, CTRL_BACK1, CTRL_FORWARD1, NUM_CTRL,
   };
   CEGUI::PushButton *_pCtrlBtn[NUM_CTRL];

   enum TeachStage {
      TEACH_NONE, TEACH_TEACHDIALOG, TEACH_PREPARE, TEACH_TEACHING, TEACH_RESULTDIALOG, TEACH_CONTDIALOG,
   };
   int _teach_stage;
   enum {
      TEACH_BEGIN_START = 0, TEACH_BEGIN_CANCEL,
      TEACH_RESULT_SUCCEED = 0, TEACH_RESULT_FAILED,
      TEACH_CONT_RETRY = 0, TEACH_CONT_NEXT, TEACH_CONT_FINISH,
   };
   ::bootes::cegui::MessageDialog *_pTeachBeginDialog, *_pTeachResultDialog, *_pTeachContDialog;
   const IMove* _pTeachMove;

   enum {
      SIDE_WIDTH = 200,
      SCREEN_MIN_WIDTH = 400,
      SCREEN_MIN_HEIGHT = 300,

      CONTROL_BUTTON_SIZE = 36,
      CONTROL_BUTTON_WIDTH = CONTROL_BUTTON_SIZE * NUM_CTRL,
      CONTROL_SLIDER_MIN_WIDTH = 100,
      CONTROL_FRAMETEXT_WIDTH = 120,
      CONTROL_MIN_WIDTH = CONTROL_BUTTON_WIDTH
                        + CONTROL_SLIDER_MIN_WIDTH
                        + CONTROL_FRAMETEXT_WIDTH,
      CONTROL_FRAMETEXT_HEIGHT = 30,
      CONTROL_HEIGHT = CONTROL_FRAMETEXT_HEIGHT * 2,
      TIMEEDIT_HEIGHT = TimeEditWindow::TIMEEDIT_HEIGHT,
   };

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
