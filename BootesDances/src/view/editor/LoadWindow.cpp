#include "LoadDialog.h"
#include <bootes/lib/util/TChar.h>

namespace bootes { namespace dances { namespace editor {

CEGUI::String LoadWindow::WidgetTypeName = "BootesDancesLoadWindow";

LoadWindow::LoadWindow(const CEGUI::String& type, const CEGUI::String& name)
  : CEGUI::LayoutContainer(type, name)
{
   _pFileList = NULL;
   _pSelectBox = NULL;
   _pCancelButton = NULL;
   _pOkButton = NULL;
   addEvent(bootes::cegui::WindowDialog::EventSubmit);
   addEvent(bootes::cegui::WindowDialog::EventCancel);
   g_pFnd->getEventManager()->subscribe< EvSearchStageResult >(this);
}

LoadWindow::~LoadWindow()
{
#define SAFE_DEL(p) if (p) { delete p; p = NULL; }
   SAFE_DEL(_pFileList);
   SAFE_DEL(_pSelectBox);
   SAFE_DEL(_pCancelButton);
   SAFE_DEL(_pOkButton);
#undef SAFE_DEL
}

namespace {
   enum {
      TEXT_HEIGHT = 24,
      FILELIST_MIN_HEIGHT = 300,
      MIN_SPACE = 5,
      MIN_HEIGHT = TEXT_HEIGHT * 3 + FILELIST_MIN_HEIGHT + MIN_SPACE * 5,

      TEXT_WIDTH = 12,
      MIN_WIDTH = TEXT_WIDTH * 30 + MIN_SPACE * 2,
      BUTTON_WIDTH = TEXT_WIDTH * 6,
   };
}

void LoadWindow::initialiseComponents()
{
   if (_pFileList) { return; }

   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();

#define CREATE_ADD(p, klass, look) \
   p = static_cast< CEGUI::klass* >(wm.createWindow("TaharezLook/" # look, ""));\
   this->addChildWindow(p);\

   CREATE_ADD(_pFileList, Listbox, Listbox);
   CREATE_ADD(_pSelectBox, Editbox, Editbox);
   CREATE_ADD(_pCancelButton, PushButton, Button);
   CREATE_ADD(_pOkButton, PushButton, Button);
#undef CREATE_ADD

   _pOkButton->setText("OK");
   _pCancelButton->setText("Cancel");

   _pFileList->subscribeEvent(CEGUI::Listbox::EventSelectionChanged,
                              CEGUI::SubscriberSlot(&LoadWindow::onSelect, this));
   _pFileList->setMultiselectEnabled(false);
   _pFileList->setShowVertScrollbar(true);
   _pFileList->setSortingEnabled(true);

   _pOkButton->subscribeEvent(CEGUI::PushButton::EventClicked,
                              CEGUI::SubscriberSlot(&LoadWindow::onOk, this));
   _pCancelButton->subscribeEvent(CEGUI::PushButton::EventClicked,
                                  CEGUI::SubscriberSlot(&LoadWindow::onCancel, this));

   setMinSize(CEGUI::UVector2(CEGUI::UDim(0,MIN_WIDTH), CEGUI::UDim(0,MIN_HEIGHT)));
}

void LoadWindow::layout()
{
   CEGUI::Size size = getPixelSize();

   // 高さ計算。余った高さは Filelist がもらう
   float filelist_height = FILELIST_MIN_HEIGHT;
   if (MIN_HEIGHT < size.d_height) {
      filelist_height += size.d_height - MIN_HEIGHT;
   }

   // 幅計算。余った幅は Dirbox,Filelist,Selectbox がもらう。
   float filelist_width = MIN_WIDTH - MIN_SPACE * 2;
   if (MIN_WIDTH < size.d_width) {
      filelist_width += size.d_width - MIN_WIDTH;
   }

   {
      CEGUI::UDim x(0,0), y(0,0), w(0,0), h(0,0);
      {
         x.d_offset = MIN_SPACE;
         y.d_offset += MIN_SPACE;
         w.d_offset = filelist_width;
         h.d_offset = filelist_height;
         _pFileList->setArea(CEGUI::UVector2(x,y), CEGUI::UVector2(w,h));
         y.d_offset += h.d_offset;
      }
      {
         x.d_offset = MIN_SPACE;
         y.d_offset += MIN_SPACE;
         w.d_offset = filelist_width;
         h.d_offset = TEXT_HEIGHT;
         _pSelectBox->setArea(CEGUI::UVector2(x,y), CEGUI::UVector2(w,h));
         y.d_offset += h.d_offset;
      }
      {
         x.d_offset = size.d_width - 2*MIN_SPACE - 2*BUTTON_WIDTH;
         y.d_offset += MIN_SPACE;
         w.d_offset = BUTTON_WIDTH;
         h.d_offset = TEXT_HEIGHT;
         _pOkButton->setArea(CEGUI::UVector2(x,y), CEGUI::UVector2(w,h));
         x.d_offset += BUTTON_WIDTH + MIN_SPACE;
         _pCancelButton->setArea(CEGUI::UVector2(x,y), CEGUI::UVector2(w,h));
         y.d_offset += h.d_offset;
      }
   }
}

bool LoadWindow::onOk(const CEGUI::EventArgs& e)
{
   if (_selected_index < 0 || _stages.size() <= _selected_index) {
      return true;
   }

   _selected_name = _T("");
   boost::shared_ptr< Stage > pStage = _stages[ _selected_index ];

   _selected_name = pStage->tc_basename;
   _stages.clear();

   CEGUI::WindowEventArgs ev(this);
   fireEvent(bootes::cegui::WindowDialog::EventSubmit, ev);
   return true;
}

bool LoadWindow::onCancel(const CEGUI::EventArgs& e)
{
   _selected_name = _T("");
   _stages.clear();

   CEGUI::WindowEventArgs ev(this);
   fireEvent(bootes::cegui::WindowDialog::EventCancel, ev);
   return true;
}

bool LoadWindow::onSelect(const CEGUI::EventArgs& e)
{
   CEGUI::ListboxItem* item = _pFileList->getFirstSelectedItem();
   if (item == NULL) { return true; }
   CEGUI::uint index = item->getID();

   if (index < 0 || _stages.size() <= index) { return true; }

   _selected_index = index;
   boost::shared_ptr< Stage > pStage = _stages[_selected_index];

   _pSelectBox->setText(pStage->name);
   _pOkButton->setEnabled(true);

   return true;
}

void LoadWindow::onSubscribe(::bootes::lib::framework::EventManager*) { }
void LoadWindow::onUnsubscribe(::bootes::lib::framework::EventManager*) { }
void LoadWindow::onEvent(const ::bootes::lib::framework::GameTime* gt, const ::bootes::lib::framework::Event* ev)
{
   if (ev->getEventId() == EvSearchStageResult::GetEventId()) {
      const EvSearchStageResult* e = static_cast< const EvSearchStageResult* >(ev);
      if (! e->_result) {
         onSearchFailed();
      } else if (! e->_pStage) {
         onSearchSucceed();
      } else {
         onSearch(e);
      }
      return;
   }
}

void LoadWindow::setup()
{
   _stages.clear();
   _pFileList->resetList();

   _pOkButton->setEnabled(false);
   _pCancelButton->setEnabled(false);
}

void LoadWindow::onSearchFailed()
{
   _stages.clear();
   _pFileList->resetList();

   _pOkButton->setEnabled(false);
   _pCancelButton->setEnabled(true);
}

void LoadWindow::onSearchSucceed()
{
   _pOkButton->setEnabled(true);
   _pCancelButton->setEnabled(true);
}

void LoadWindow::onSearch(const EvSearchStageResult* e)
{
   CEGUI::uint id = _stages.size();
   _stages.push_back(e->_pStage);

   {
      CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(e->_pStage->basename, id, NULL);
      item->setSelectionColours(CEGUI::ColourRect(CEGUI::colour(1.0f, 0.0f, 0.0f)));
      _pFileList->addItem(item);
   }

   _pOkButton->setEnabled(false);
   _pCancelButton->setEnabled(true);
}

} } }

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
