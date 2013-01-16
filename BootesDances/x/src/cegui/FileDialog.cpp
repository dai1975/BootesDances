#include <bootes/cegui/FileDialog.h>
#include <bootes/lib/util/TChar.h>
#include "FileDialogLayout.h"

namespace bootes { namespace cegui {

const CEGUI::String FileDialog::EventSubmit    = "BootesCeguiFileDialogLayout_EventSubmit";
const CEGUI::String FileDialog::EventCancel    = "BootesCeguiFileDialogLayout_EventCancel";
      CEGUI::String FileDialog::WidgetTypeName = "BootesCeguiFileDialog";

FileDialog::FileDialog(const CEGUI::String& type, const CEGUI::String& name)
   : ::CEGUI::FrameWindow(type, name)
{
   _pLayout = NULL;
   addEvent(FileDialog::EventSubmit);
   addEvent(FileDialog::EventCancel);
}

FileDialog::~FileDialog()
{
   if (_pLayout) { delete _pLayout; }
}

FileDialog* FileDialog::Create(const CEGUI::String& name)
{
   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();
/*
   CEGUI::WindowFactoryManager::addFactory< CEGUI::TplWindowFactory< FileDialog > >();

   CEGUI::WindowFactoryManager& wfm = CEGUI::WindowFactoryManager::getSingleton();
   wfm.addFalagardWindowMapping("FileDialog", FileDialog::WidgetTypeName, "TaharezLook/FrameWindow", "Falagard/FrameWindow");
   
   //FileDialog* p = static_cast< FileDialog* >(wm.createWindow(FileDialog::WidgetTypeName, name));
   */
   FileDialog* p = static_cast< FileDialog* >(wm.createWindow("FileDialog", name));
   return p;
}

void FileDialog::initialiseComponents()
{
   if (_pLayout) { return; }

   setText("");
   setCloseButtonEnabled(false);
   //setTitleBarEnabled(false);

   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();
   _pLayout = static_cast< FileDialogLayout* >(wm.createWindow(FileDialogLayout::WidgetTypeName, ""));
   _pLayout->initialiseComponents(); //Falagard 経由じゃないと呼ばれない?ので手動で
   this->addChildWindow(_pLayout);

   {
      CEGUI::Size s0    = this->getParentPixelSize();
      CEGUI::Vector2 s = _pLayout->getMinSize().asAbsolute(s0);
      CEGUI::UDim x(0,0), y(0,0);

      x.d_offset = s.d_x + 30;
      y.d_offset = s.d_y + 60;
      this->setMinSize(CEGUI::UVector2(x,y));
      x.d_offset = (s0.d_width  - x.d_offset) / 2;
      y.d_offset = (s0.d_height - y.d_offset) / 2;
      this->setPosition(CEGUI::UVector2(x,y));
   }
}

void FileDialog::dialog(const TCHAR* initdir, const TCHAR** exts)
{
   if (_pLayout == NULL) { return; }
   //if (isVisible(true)) { return; }

   _pLayout->dialog(initdir, exts);
   setModalState(true);
   show();
}

const TCHAR* FileDialog::getDir() const {
   return (_pLayout == NULL)?NULL: _pLayout->getDir();
}
const TCHAR* FileDialog::getFile() const {
   return (_pLayout == NULL)?NULL: _pLayout->getFile();
}

void FileDialog::onSubmit()
{
   setModalState(false);
   hide();
   CEGUI::WindowEventArgs ev(this);
   fireEvent(FileDialog::EventSubmit, ev);
}

void FileDialog::onCancel()
{
   setModalState(false);
   hide();
   CEGUI::WindowEventArgs ev(this);
   fireEvent(FileDialog::EventCancel, ev);
}

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
