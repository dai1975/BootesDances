#include <bootes/cegui/cegui.h>
#include <bootes/cegui/DialogWindow.h>
#include "DialogWindowLayout.h"
#include <bootes/cegui/FileDialog.h>
#include "FileDialogLayout.h"

namespace bootes { namespace cegui {

bool Initialize()
{
   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();
   CEGUI::WindowFactoryManager& wfm = CEGUI::WindowFactoryManager::getSingleton();

   CEGUI::WindowFactoryManager::addFactory< CEGUI::TplWindowFactory< DialogWindow > >();
   wfm.addFalagardWindowMapping("DialogWindow", DialogWindow::WidgetTypeName, "TaharezLook/FrameWindow", "Falagard/FrameWindow");
   CEGUI::WindowFactoryManager::addFactory< CEGUI::TplWindowFactory< DialogWindowLayout > >();

   CEGUI::WindowFactoryManager::addFactory< CEGUI::TplWindowFactory< FileDialog > >();
   wfm.addFalagardWindowMapping("FileDialog", FileDialog::WidgetTypeName, "TaharezLook/FrameWindow", "Falagard/FrameWindow");
   CEGUI::WindowFactoryManager::addFactory< CEGUI::TplWindowFactory< FileDialogLayout > >();
//   wfm.addFalagardWindowMapping("FileDialogLayout", FileDialog::WidgetTypeName, "TaharezLook/FrameWindow", "Falagard/Default");

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
