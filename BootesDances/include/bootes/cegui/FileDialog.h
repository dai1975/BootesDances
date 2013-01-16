#ifndef BOOTESDANCES_CEGUI_FILEDIALOG_H
#define BOOTESDANCES_CEGUI_FILEDIALOG_H

#include <CEGUI.h>
#include <TCHAR.h>

namespace bootes { namespace cegui {

class FileDialogLayout;
class FileDialog: public CEGUI::FrameWindow
{
public:
   static FileDialog* Create(const CEGUI::String& name);

   static CEGUI::String WidgetTypeName;
   static const CEGUI::String EventSubmit;
   static const CEGUI::String EventCancel;

   FileDialog(const CEGUI::String& type, const CEGUI::String& name);
   virtual ~FileDialog();
   virtual void initialiseComponents();
public:
   void dialog(const TCHAR* initdir, const TCHAR** exts);
   const TCHAR* getDir() const;
   const TCHAR* getFile() const;

private:
   friend class FileDialogLayout;
   void onSubmit();
   void onCancel();
   FileDialogLayout* _pLayout;
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
