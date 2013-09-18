#ifndef BOOTESDANCES_EDITOR_LOADDIALOG_H
#define BOOTESDANCES_EDITOR_LOADDIALOG_H

#include "../../include.h"
#include <bootes/cegui/WindowDialog.h>
#include <bootes/dances/EvStage.h>

namespace bootes { namespace dances { namespace editor {

class LoadWindow: public CEGUI::LayoutContainer
                , public ::bootes::lib::framework::EventListener
{
public:
   static CEGUI::String WidgetTypeName;
   LoadWindow(const CEGUI::String& type, const CEGUI::String& name);
   virtual ~LoadWindow();
   virtual void initialiseComponents();
   virtual void layout();

public:
   void setup();
   inline const TCHAR* getSelectedName() const { return _selected_name.c_str(); }

private:
   bool onSelect(const CEGUI::EventArgs& e);
   bool onOk(const CEGUI::EventArgs& e);
   bool onCancel(const CEGUI::EventArgs& e);

public:
   void onSubscribe(::bootes::lib::framework::EventManager*);
   void onUnsubscribe(::bootes::lib::framework::EventManager*);
   void onEvent(const ::bootes::lib::framework::GameTime* gt, const ::bootes::lib::framework::Event* ev);
private:
   void onSearchFailed();
   void onSearchSucceed();
   void onSearch(const EvSearchStageResult*);

private:
   CEGUI::Listbox *_pFileList;
   CEGUI::Editbox *_pSelectBox;
   CEGUI::PushButton *_pCancelButton, *_pOkButton;

   std::basic_string< TCHAR > _selected_name;
   intptr_t _selected_index;
   std::vector< boost::shared_ptr< Stage > > _stages;
};
typedef bootes::cegui::Dialog< LoadWindow > LoadDialog;

} } }

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
