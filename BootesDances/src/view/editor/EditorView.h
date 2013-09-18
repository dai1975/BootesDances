#ifndef BOOTESDANCES_VIEW_CEGUI_H
#define BOOTESDANCES_VIEW_CEGUI_H

#include "../../include.h"
#include <DirectShowTextureExport.h>
#include "CeguiTextureImage.h"
#include "LoadDialog.h"
#include "EditWindow.h"
#include <bootes/cegui/FileDialog.h>

#pragma warning(disable : 4995)
#include <CEGUI.h>
#include <RendererModules/Direct3D9/CEGUIDirect3D9Renderer.h>
#include <RendererModules/Direct3D9/CEGUIDirect3D9Texture.h>

namespace bootes { namespace dances { namespace editor {

class EditorView
   : public ::bootes::lib::framework::View
   , public ::bootes::lib::framework::EventListener
{
public:
   explicit EditorView();
   virtual ~EditorView();

public:
   virtual void onUpdate(const GameTime* gt);
   virtual void onRender(const GameTime* gt);
   virtual bool onSensorInput(const GameTime* gt, const InputEvent*);
   virtual bool onInput(const GameTime* gt, const InputEvent*);
   virtual void onLostDevice();
   virtual void onResetDevice();

public:
   virtual void onSubscribe(::bootes::lib::framework::EventManager*);
   virtual void onUnsubscribe(::bootes::lib::framework::EventManager*);
   virtual void onEvent(const ::bootes::lib::framework::GameTime*, const ::bootes::lib::framework::Event* ev);

   bool onMenuFileNew(const CEGUI::EventArgs&);
   bool onMenuFileLoad(const CEGUI::EventArgs&);
   bool onMenuFileSave(const CEGUI::EventArgs&);

   bool onNewDialogSubmit(const CEGUI::EventArgs&);
   bool onNewDialogCancel(const CEGUI::EventArgs&);
   bool onLoadDialogSubmit(const CEGUI::EventArgs&);
   bool onLoadDialogCancel(const CEGUI::EventArgs&);

   void onLoad(bool result, const TCHAR* basename, bool neu);

protected:
   void onMouseEnter();
   void onMouseLeave();

private:
   void onInitDevice();
   void createSheet();
   void createSheet0();

   enum STATE {
      S_0, S_RUN,
   };
   STATE _state;
   
   CEGUI::Direct3D9Renderer* _pCeguiRenderer;
   bool _bMouseEnter;
   CEGUI::Window *_pWRoot, *_pWTheater, *_pWFile;
   EditWindow* _pEditWindow;
   bootes::cegui::FileDialog* _pNewDialog;
   LoadDialog* _pLoadDialog;

   CeguiTextureImage _image;
   int _elapse;
   int _mode;
   std::basic_string<TCHAR> _stage_basename;
   bool _stage_neu;
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
