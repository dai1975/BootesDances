#include <bootes/cegui/cegui.h>
#include <bootes/cegui/WindowDialog.h>
#include <bootes/cegui/MessageDialog.h>
#include <bootes/cegui/FileDialog.h>
//#include "MessageWindowLayout.h"

namespace bootes { namespace cegui {

bool Initialize()
{
   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();
   CEGUI::WindowFactoryManager& wfm = CEGUI::WindowFactoryManager::getSingleton();
   CEGUI::String* pType;

   CEGUI::WindowFactoryManager::addFactory< CEGUI::TplWindowFactory< WindowDialog > >();
   wfm.addFalagardWindowMapping("WindowDialog", WindowDialog::WidgetTypeName, "TaharezLook/FrameWindow", "Falagard/FrameWindow");

//   CEGUI::WindowFactoryManager::addFactory< CEGUI::TplWindowFactory< DialogWindow > >();
//   wfm.addFalagardWindowMapping("DialogWindow", DialogWindow::WidgetTypeName, "TaharezLook/FrameWindow", "Falagard/FrameWindow");
//   CEGUI::WindowFactoryManager::addFactory< CEGUI::TplWindowFactory< DialogWindowLayout > >();

#define SETUP_WIDGET(T) {\
      pType = & T ::WidgetTypeName;\
      CEGUI::WindowFactoryManager::addFactory< CEGUI::TplWindowFactory< T > >(); \
      wfm.addFalagardWindowMapping(*pType, *pType, "TaharezLook/FrameWindow", "Falagard/FrameWindow");\
   }
   SETUP_WIDGET( MessageWindow );
//   SETUP_WIDGET( MessageWindowLayout );
   SETUP_WIDGET( FileWindow );

/*
#define SETUP_DIALOG(T) {\
      pType = & T ::WidgetTypeName;\
      CEGUI::WindowFactoryManager::addFactory< CEGUI::TplWindowFactory< T::WINDOW_TYPE > >(); \
      wfm.addFalagardWindowMapping(*pType, *pType, "TaharezLook/FrameWindow", "Falagard/FrameWindow");\
   }

   SETUP_DIALOG(MessageDialog);
   SETUP_DIALOG(FileDialog);
*/

   return true;
}  

void Finalize() { }

} }

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
