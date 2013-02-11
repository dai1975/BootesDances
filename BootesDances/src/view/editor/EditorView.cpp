#include "EditorView.h"
#include <bootes/cegui/cegui.h>
#include <bootes/cegui/DynamicFont.h>
#include <bootes/dances/EvStage.h>
#include <bootes/lib/util/TChar.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <io.h> //_close()
#include <sys/stat.h>
#include <sys/types.h>
#include <share.h>
#include <fcntl.h>

namespace bootes { namespace dances { namespace editor {

EditorView::EditorView()
: View()
, _image()
{
   _state = S_0;
   _bMouseEnter = false;
   _pFileDialog = NULL;
   _pEditWindow = NULL;
   _pCeguiRenderer = NULL;
   _elapse = 0;
   _mode = 0;

   g_pFnd->getEventManager()->subscribe< EvNewStageResult >(this);
   g_pFnd->getEventManager()->subscribe< EvLoadStageResult >(this);
   g_pFnd->getEventManager()->subscribe< EvSaveStageResult >(this);
}

EditorView::~EditorView()
{
   if (_pFileDialog) { delete _pFileDialog; }
   if (_pEditWindow) { delete _pEditWindow; }
   if (_pCeguiRenderer) {
      CEGUI::Direct3D9Renderer::destroy(*_pCeguiRenderer);
      _pCeguiRenderer = NULL;
   }
   ::bootes::cegui::Finalize();
}

void EditorView::onResetDevice()
{
   if (_pCeguiRenderer == NULL) {
      onInitDevice();
      _image.onInitDevice();
      createSheet();
      _pCeguiRenderer->postD3DReset();

   } else {
      _pCeguiRenderer->postD3DReset();
      _image.onResetDevice();
   }
}

void EditorView::onLostDevice()
{
   _pCeguiRenderer->preD3DReset();
}

void EditorView::onInitDevice()
{
   //HRESULT hr;
   IDirect3DDevice9* pDev = g_pFnd->getD3D9Device();

   _pCeguiRenderer = &CEGUI::Direct3D9Renderer::bootstrapSystem(pDev);

   if (! ::bootes::cegui::Initialize()) { return; }
   
   CEGUI::DefaultResourceProvider *rp = static_cast< CEGUI::DefaultResourceProvider* >
      (CEGUI::System::getSingleton().getResourceProvider());
   
   rp->setResourceGroupDirectory("schemes", "datafiles/schemes/");
   rp->setResourceGroupDirectory("imagesets", "datafiles/imagesets/");
   rp->setResourceGroupDirectory("fonts", "datafiles/fonts/");
   rp->setResourceGroupDirectory("layouts", "datafiles/layouts/");
   rp->setResourceGroupDirectory("looknfeel", "datafiles/looknfeel/");
   rp->setResourceGroupDirectory("lua_scripts", "datafiles/lua_scripts/");
   rp->setResourceGroupDirectory("xml_schemas", "datafiles/xml_schemas/");

   CEGUI::Imageset::setDefaultResourceGroup("imagesets");
   CEGUI::Font::setDefaultResourceGroup("fonts");
   CEGUI::Scheme::setDefaultResourceGroup("schemes");
   CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeel");
   CEGUI::WindowManager::setDefaultResourceGroup("layouts");
   CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
   CEGUI::XMLParser* parser = CEGUI::System::getSingleton().getXMLParser();
   if (parser->isPropertyPresent("SchemaDefaultResourceGroup")) {
      parser->setProperty("SchemaDefaultResourceGroup", "xml_schemas");
   }
}

void EditorView::createSheet()
{
   try {
/*
		CEGUI::WindowFactoryManager& wfm = CEGUI::WindowFactoryManager::getSingleton();
		wfm.addFactory< CEGUI::TplWindowFactory< SurfaceWindow > >();
		wfm.addFalagardWindowMapping("TaharezLook/SurfaceWindow", "SurfaceWindow", "TaharezLook/FrameWindow", "Falagard/FrameWindow");
*/
      CEGUI::WindowFactoryManager& wfm = CEGUI::WindowFactoryManager::getSingleton();
      wfm.addFactory< CEGUI::TplWindowFactory< EditWindow > >();
      //wfm.addWindowTypeAlias("DFX/EditorWindow", 
//		wfm.addFalagardWindowMapping("TaharezLook/DfxEditWindow", "DfxEditWindow", "TaharezLook/FrameWindow", "Falagard/Default");

      CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
      CEGUI::System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
      CEGUI::System::getSingleton().setDefaultTooltip("TaharezLook/Tooltip");
      CEGUI::FontManager::getSingleton().create("DejaVuSans-10.font");

      //CEGUI::System::getSingleton().setDefaultFont("DejaVuSans-10");
      {
         CEGUI::Font* font = ::bootes::cegui::DynamicFont::GetAsciiFont(8);
         CEGUI::System::getSingleton().setDefaultFont(font);
      }
      
      createSheet0();

   } catch (CEGUI::Exception& e) {
      const char* msg = e.getMessage().c_str();
      const char* filename = e.getFileName().c_str();
      const char* name = e.getName().c_str();
      int line = e.getLine();
      DebugBreak();
   }
}

void EditorView::createSheet0()
{
   bool bAutoRoot  = false;
   bool bAutoFrame = false;
   bool bAutoImage = false;
   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();
   
#define UV2(xs,xo,ys,yo) CEGUI::UVector2(CEGUI::UDim((float)xs,(float)xo), CEGUI::UDim((float)ys,(float)yo))

   int width  = 1920/3;
   int height = 1080/3;
   { //Theator
      _pEditWindow = static_cast< EditWindow*>(wm.createWindow("BootesDancesEditWindow", (CEGUI::utf8*)"EditWindow"));
      if (! _pEditWindow->init(&_image)) {
         DbgBreak("EditWindow#init");
         return;
      }
   }

   _pWRoot = wm.loadWindowLayout("RootWindow.layout");
   _pWRoot->setUsingAutoRenderingSurface(bAutoRoot);
   _pWRoot->setSize(UV2(1.0, 0, 1.0, 0));

   CEGUI::Window *pWMain = _pWRoot->getChild("Root/MainWindow");
   pWMain->addChildWindow(_pEditWindow);

   _pFileDialog = ::bootes::cegui::FileDialog::Create("FileDialog");
   _pFileDialog->subscribeEvent(::bootes::cegui::FileDialog::EventSubmit,
                                CEGUI::SubscriberSlot(&EditorView::onDialogSubmit, this));
   _pFileDialog->subscribeEvent(::bootes::cegui::FileDialog::EventCancel,
                                CEGUI::SubscriberSlot(&EditorView::onDialogCancel, this));
   _pWRoot->addChildWindow(_pFileDialog);
   _pFileDialog->hide();

   {
      wm.getWindow("Root/Menubar/File/New")->setEnabled(true);
      wm.getWindow("Root/Menubar/File/Load")->setEnabled(true);
      wm.getWindow("Root/Menubar/File/Save")->setEnabled(false);
      wm.getWindow("Root/Menubar/File/Exit")->setEnabled(false);

      wm.getWindow("Root/Menubar/File/New")->subscribeEvent(
         CEGUI::MenuItem::EventClicked,
         CEGUI::Event::Subscriber(&EditorView::onMenuFileNew, this));
      wm.getWindow("Root/Menubar/File/Load")->subscribeEvent(
         CEGUI::MenuItem::EventClicked,
         CEGUI::Event::Subscriber(&EditorView::onMenuFileLoad, this));
      wm.getWindow("Root/Menubar/File/Save")->subscribeEvent(
         CEGUI::MenuItem::EventClicked,
         CEGUI::Event::Subscriber(&EditorView::onMenuFileSave, this));
   }

   CEGUI::System::getSingleton().setGUISheet(_pWRoot);
}

bool EditorView::onMenuFileNew(const CEGUI::EventArgs& ev)
{
   _pFileDialog->setText("Open New Movie");
   intptr_t mode = DIALOG_NEW;
   _pFileDialog->setUserData(reinterpret_cast< void* >(mode));
   _pFileDialog->setAlwaysOnTop(true);
   const TCHAR* exts[] = { _T(".mp4"),_T(".avi"),_T("mpeg"),_T("mov"),_T("mkv"), 0 };
   _pFileDialog->dialog( g_pGame->getUserDir(), exts);
   return true;
}

bool EditorView::onMenuFileLoad(const CEGUI::EventArgs& ev)
{
/*
   _pFileDialog->setText("Open Stage File");
   intptr_t mode = DIALOG_LOAD;
   _pFileDialog->setUserData(reinterpret_cast< void* >(mode));
   _pFileDialog->setAlwaysOnTop(true);
   const TCHAR* exts[] = { _T(".boo"), 0, };
   _pFileDialog->dialog( g_pGame->getUserDir(), exts);
   return true;
*/
   return false;
}

bool EditorView::onMenuFileSave(const CEGUI::EventArgs& ceguiev)
{
   EvSaveStage ev;
   ev._basename = _stage_basename;
   ev._new      = _stage_neu;
   g_pFnd->queue(&ev);
   return true;
}

bool EditorView::onDialogSubmit(const CEGUI::EventArgs& ev)
{
   const CEGUI::WindowEventArgs& wev = static_cast< const CEGUI::WindowEventArgs& >(ev);
   intptr_t mode = reinterpret_cast< intptr_t >( wev.window->getUserData() );

   const TCHAR* tc_dir  = _pFileDialog->getDir();
   const TCHAR* tc_file = _pFileDialog->getFile();
   if (tc_dir == NULL || tc_dir[0] == _T('\0')) { return true; }
   if (tc_file == NULL || tc_file[0] == _T('\0')) { return true; }

   switch (mode) {
   case DIALOG_NEW:  doDialogNew(tc_dir, tc_file); break;
   case DIALOG_LOAD: doDialogLoad(tc_dir, tc_file); break;

      //   case DIALOG_SAVE: doSave(tc_dir, tc_file); break;
   default:
      return true;
   }
   return true;
}

bool EditorView::onDialogCancel(const CEGUI::EventArgs& ev)
{
   return true;
}

void EditorView::doDialogNew(const TCHAR* tc_dir, const TCHAR* tc_file)
{
   EvNewStage ev;
   ev._moviepath.append(tc_dir).append(_T("\\")).append(tc_file);
   g_pFnd->getEventManager()->queue(&ev);
}

void EditorView::doDialogLoad(const TCHAR* dir, const TCHAR* file)
{
   const TCHAR *pc;
   for (pc = dir; pc != _T('\0'); ++pc) {
      if (*pc == _T('\\')) { break; }
   }
   if (*pc == _T('\0')) { pc = dir; }

   EvLoadStage ev;
   ev._basename = pc;
   g_pFnd->getEventManager()->queue(&ev);
}


void EditorView::onUpdate(double currentTime, int elapsedTime)
{
   switch (_state) {
   case S_0:
      {
         _state = S_RUN;
      }
      break;
   case S_RUN:
      break;
   }
   _pEditWindow->onUpdate(currentTime, elapsedTime);
   float ms = ((float)elapsedTime) / (float)1000;
   CEGUI::System::getSingleton().injectTimePulse(ms);
}

void EditorView::onRender(double currentTime, int elapsedTime)
{
   _image.onRender(currentTime, elapsedTime);
   _pEditWindow->onRender(currentTime, elapsedTime);
   CEGUI::System::getSingleton().renderGUI();
}

void EditorView::onMouseEnter()
{
   if (!_bMouseEnter) {
      _bMouseEnter = true;
      ShowCursor(false);
   }
}
void EditorView::onMouseLeave()
{
   if (_bMouseEnter) {
      _bMouseEnter = false;
      ShowCursor(true);
   }
}

bool EditorView::onInput(const ::bootes::lib::framework::InputEvent* ev)
{
   switch (ev->_type) {
   case ::bootes::lib::framework::InputEvent::T_WNDMSG:
      {
         const ::bootes::lib::framework::WndmsgEvent* we = static_cast< const ::bootes::lib::framework::WndmsgEvent* >(ev);
         CEGUI::System& sys = CEGUI::System::getSingleton();
         switch (we->_message) {
         case WM_KEYDOWN:
         case WM_KEYUP:
            return false;
               
         case WM_CHAR:
            return sys.injectChar((CEGUI::utf32)we->_wParam);
               
         case WM_MOUSELEAVE:
         case WM_NCMOUSEMOVE:
            onMouseLeave();
            return false;
            
         case WM_MOUSEMOVE:
            return sys.injectMousePosition((float)LOWORD(we->_lParam), (float)HIWORD(we->_lParam));
         case WM_LBUTTONDOWN:
            return sys.injectMouseButtonDown(CEGUI::LeftButton);
         case WM_LBUTTONUP:
            return sys.injectMouseButtonUp(CEGUI::LeftButton);
         case WM_RBUTTONDOWN:
            return sys.injectMouseButtonDown(CEGUI::RightButton);
         case WM_RBUTTONUP:
            return sys.injectMouseButtonUp(CEGUI::RightButton);
         case WM_MBUTTONDOWN:
            return sys.injectMouseButtonDown(CEGUI::MiddleButton);
         case WM_MBUTTONUP:
            return sys.injectMouseButtonUp(CEGUI::MiddleButton);
            
         case 0x020A: //wheel
            return sys.injectMouseWheelChange
               (static_cast<float>((short)HIWORD(we->_wParam)) / static_cast<float>(120));
         default:
            return false;
         }
      }
      break;
   case ::bootes::lib::framework::InputEvent::T_WIIMOTE:
      {
         const ::bootes::lib::framework::WiimoteEvent* we = static_cast< const ::bootes::lib::framework::WiimoteEvent* >(ev);
         _pEditWindow->onInput(we);
      }
   }
   return false;
}

void EditorView::onSubscribe(::bootes::lib::framework::EventManager*) { }
void EditorView::onUnsubscribe(::bootes::lib::framework::EventManager*) { }
void EditorView::onEvent(const ::bootes::lib::framework::Event* ev)
{
   if (ev->getEventId() == EvLoadStageResult::GetEventId()) {
      const EvLoadStageResult* e = static_cast< const EvLoadStageResult* >(ev);
      onLoad(e->_result, e->_basename.c_str(), false);
      return;
   } else if (ev->getEventId() == EvNewStageResult::GetEventId()) {
      const EvNewStageResult* e = static_cast< const EvNewStageResult* >(ev);
      onLoad(e->_result, e->_basename.c_str(), true);
      return;
   } else if (ev->getEventId() == EvSaveStageResult::GetEventId()) {
      const EvSaveStageResult* e = static_cast< const EvSaveStageResult* >(ev);
      if (e->_result) {
         _stage_basename = e->_basename;
         _stage_neu = false;
      }
      return;
   }
}

void EditorView::onLoad(bool result, const TCHAR* basename, bool neu)
{
   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();
   if (result) {
      wm.getWindow("Root/Menubar/File/Save")->setEnabled(true);
      _stage_basename = basename;
      _stage_neu      = neu;
   } else {
      wm.getWindow("Root/Menubar/File/Save")->setEnabled(false);
      _stage_basename = _T("");
      _stage_neu      = true;
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
