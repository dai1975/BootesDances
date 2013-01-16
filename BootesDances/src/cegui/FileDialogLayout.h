#ifndef BOOTESDANCES_CEGUI_FILEDIALOGLAYOUT_H
#define BOOTESDANCES_CEGUI_FILEDIALOGLAYOUT_H

#include "../include.h"
#include <CEGUI.h>

namespace bootes { namespace cegui {

class FileDialogLayout: public CEGUI::LayoutContainer
{
public:
   static CEGUI::String WidgetTypeName;
   FileDialogLayout(const CEGUI::String& type, const CEGUI::String& name);
   virtual ~FileDialogLayout();
   virtual void initialiseComponents();
   virtual void layout();

public:
   void dialog(const TCHAR* initdir, const TCHAR** exts);

   bool onSelect(const CEGUI::EventArgs& e);
   bool onOk(const CEGUI::EventArgs& e);
   bool onCancel(const CEGUI::EventArgs& e);
   inline const TCHAR* getDir() const { return _cwd.c_str(); }
   inline const TCHAR* getFile() const { return _select_file.c_str(); }

private:
   int list(const TCHAR* dir);
   
   CEGUI::Listbox *_pFileList;
   CEGUI::Editbox *_pDirBox, *_pSelectBox;
   CEGUI::PushButton *_pCancelButton, *_pOkButton;
   std::basic_string< TCHAR > _cwd;
   std::basic_string< TCHAR > _select_file;
   std::list< std::basic_string< TCHAR > > _exts;
};

} }

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
