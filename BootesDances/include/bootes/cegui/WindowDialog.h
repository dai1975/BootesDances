#ifndef BOOTESDANCES_CEGUI_WINDOWDIALOG_H
#define BOOTESDANCES_CEGUI_WINDOWDIALOG_H

#include <CEGUI.h>

namespace bootes { namespace cegui {

class WindowDialog: public CEGUI::FrameWindow
{
public:
   static WindowDialog* Create(const CEGUI::String& name, const CEGUI::String& childType);

   static CEGUI::String WidgetTypeName;
   static const CEGUI::String EventSubmit;
   static const CEGUI::String EventCancel;

   WindowDialog(const CEGUI::String& type, const CEGUI::String& name);
   virtual ~WindowDialog();
   virtual void initialiseComponents();

public:
   void setChildType(const CEGUI::String& childName);
   CEGUI::Window* getChildWindow();
   void open();

protected:
   virtual CEGUI::Window* createChildWindow(const CEGUI::String& child_type);

private:
   bool onSubmit(const CEGUI::EventArgs&);
   bool onCancel(const CEGUI::EventArgs&);
   CEGUI::Window* _pChild;
   CEGUI::String _child_type;
};

template <typename T> class Dialog: public WindowDialog
{
   typedef Dialog< T > SELF;
public:
   typedef T WINDOW_TYPE;

   static CEGUI::String WidgetTypeName;

   inline Dialog(const CEGUI::String& type, const CEGUI::String& name)
      : WindowDialog(type, name)
      { }

   inline static SELF* Create(const CEGUI::String& name) {
      static bool first = true;
      if (first) {
         first = false;
         SELF::WidgetTypeName = T::WidgetTypeName + "_BootesDialog";

         CEGUI::WindowFactoryManager& wfm = CEGUI::WindowFactoryManager::getSingleton();
         CEGUI::WindowFactoryManager::addFactory< CEGUI::TplWindowFactory< SELF > >();
         wfm.addFalagardWindowMapping(SELF::WidgetTypeName, SELF::WidgetTypeName, "TaharezLook/FrameWindow", "Falagard/FrameWindow");
      }
      CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();
      SELF* p = static_cast< SELF* >(wm.createWindow(SELF::WidgetTypeName, name));
      p->setChildType( T::WidgetTypeName );
      return p;
   }

   inline virtual void initialiseComponents() {
      setChildType(T::WidgetTypeName);
      WindowDialog::initialiseComponents();
   }

   inline T* window() {
      return static_cast< T* >(getChildWindow());
   }
};
template <typename T> CEGUI::String Dialog< T >::WidgetTypeName = T::WidgetTypeName + "_BootesDialog";

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
