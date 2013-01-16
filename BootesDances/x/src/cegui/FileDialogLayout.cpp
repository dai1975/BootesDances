#include "FileDialogLayout.h"
#include <bootes/cegui/FileDialog.h>
#include <bootes/lib/util/TChar.h>

namespace bootes { namespace cegui {

CEGUI::String FileDialogLayout::WidgetTypeName = "FileDialogLayout";

FileDialogLayout::FileDialogLayout(const CEGUI::String& type, const CEGUI::String& name)
  : CEGUI::LayoutContainer(type, name)
{
   _pFileList = NULL;
   _pDirBox = NULL;
   _pSelectBox = NULL;
   _pCancelButton = NULL;
   _pOkButton = NULL;
}

FileDialogLayout::~FileDialogLayout()
{
#define SAFE_DEL(p) if (p) { delete p; p = NULL; }
   SAFE_DEL(_pFileList);
   SAFE_DEL(_pDirBox);
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

void FileDialogLayout::initialiseComponents()
{
   if (_pFileList) { return; }

   CEGUI::WindowManager& wm = CEGUI::WindowManager::getSingleton();

#define CREATE_ADD(p, klass, look) \
   p = static_cast< CEGUI::klass* >(wm.createWindow("TaharezLook/" # look, ""));\
   this->addChildWindow(p);\

   CREATE_ADD(_pFileList, Listbox, Listbox);
   CREATE_ADD(_pDirBox, Editbox, Editbox);
   CREATE_ADD(_pSelectBox, Editbox, Editbox);
   CREATE_ADD(_pCancelButton, PushButton, Button);
   CREATE_ADD(_pOkButton, PushButton, Button);
#undef CREATE_ADD

   _pOkButton->setText("OK");
   _pCancelButton->setText("Cancel");

   _pFileList->subscribeEvent(CEGUI::Listbox::EventSelectionChanged,
                              CEGUI::SubscriberSlot(&FileDialogLayout::onSelect, this));
   _pFileList->setMultiselectEnabled(false);
   _pFileList->setShowVertScrollbar(true);

   _pOkButton->subscribeEvent(CEGUI::PushButton::EventClicked,
                              CEGUI::SubscriberSlot(&FileDialogLayout::onOk, this));
   _pCancelButton->subscribeEvent(CEGUI::PushButton::EventClicked,
                                  CEGUI::SubscriberSlot(&FileDialogLayout::onCancel, this));

   setMinSize(CEGUI::UVector2(CEGUI::UDim(0,MIN_WIDTH), CEGUI::UDim(0,MIN_HEIGHT)));
}

void FileDialogLayout::layout()
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
         h.d_offset = TEXT_HEIGHT;
         _pDirBox->setArea(CEGUI::UVector2(x,y), CEGUI::UVector2(w,h));
         y.d_offset += h.d_offset;
      }
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

bool FileDialogLayout::onOk(const CEGUI::EventArgs& e)
{
   _select_file = _T("");

   if (_cwd.empty()) { return true; }

   const char* file = _pSelectBox->getText().c_str();
   if (file == NULL || file[0] == '\0') { return true; }

   TCHAR* tc_file = ::bootes::lib::util::TChar::C2T(file);
   if (tc_file == NULL) { return true; }
   
   _select_file = tc_file;
   delete[] tc_file;
   static_cast< FileDialog* >(getParent())->onSubmit();
   return true;
}

bool FileDialogLayout::onCancel(const CEGUI::EventArgs& e)
{
   _select_file = _T("");
   static_cast< FileDialog* >(getParent())->onCancel();
   return true;
}

bool FileDialogLayout::onSelect(const CEGUI::EventArgs& e)
{
   CEGUI::ListboxItem* item = _pFileList->getFirstSelectedItem();
   if (item == NULL) { return true; }
   intptr_t data = reinterpret_cast< intptr_t >(item->getUserData());

   if (data == 0) { //file
      _pSelectBox->setText(item->getText());
      _pOkButton->setEnabled(true);
   } else if (data == 1) { //dir
      TCHAR* name = ::bootes::lib::util::TChar::C2T(item->getText().c_str());
      tc_string dir = _cwd + _T('\\') + name;
      delete name;
      list(dir.c_str());
   }
   return true;
}

int FileDialogLayout::list(const TCHAR* dir)
{
   typedef std::basic_string<TCHAR> tc_string;
   typedef std::list< tc_string >   tc_string_list;

   _pFileList->resetList();
   _pSelectBox->setText("");
   _pOkButton->setEnabled(false);

   if (dir[0] == 0) {
      _cwd = _T("C:");
   } else {
      size_t i = 1;
      while (dir[i] != 0) { ++i; }
      while (0 < i) {
         if (dir[i-1] != _T('\\') && dir[i-1] != _T('/')) {
            break;
         }
         --i;
      }
      _cwd.assign(dir, i);
   }

   {
      char* s = ::bootes::lib::util::TChar::T2C(_cwd.c_str());
      _pDirBox->setText(s);
      delete s;
   }

   WIN32_FIND_DATA find;
   HANDLE hFind;
   {
      tc_string query = _cwd + _T("\\*");
      hFind = FindFirstFile(query.c_str(), &find);
      if (hFind == INVALID_HANDLE_VALUE) {
         return -1;
      }
   }

   tc_string_list files, dirs;
   for (BOOL found = TRUE; found; found=FindNextFile(hFind, &find)) {
      tc_string name(find.cFileName);
      if (name.compare(_T(".")) == 0) { continue; }

      if ((find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
         name += _T('\\');
         dirs.push_back(name);
      } else if ((find.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) != 0) {
         continue;
      } else {
         std::list< tc_string >::iterator i;
         for (i = _exts.begin(); i != _exts.end(); ++i) {
            tc_string& e = *i;
            if (name.size() < e.size()) { continue; }
            if (name.compare(name.size() - e.size(), e.size(), e) == 0) { break; }
         }
         if (i == _exts.end()) { continue; }
         files.push_back(name);
      }
   }
   FindClose(hFind);


   int id = 0;
   intptr_t data = 1; //dir
   void* pdata = reinterpret_cast< void* >(data);
   for (tc_string_list::iterator i = dirs.begin(); i != dirs.end(); ++i) {
      char* name = ::bootes::lib::util::TChar::T2C(i->c_str());
      CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(name, id++, pdata);
      item->setSelectionColours(CEGUI::ColourRect(CEGUI::colour(1.0f, 0.0f, 0.0f)));
      _pFileList->addItem(item);
      delete name;
   }

   data = 0; //file
   pdata = reinterpret_cast< void* >(data);
   for (tc_string_list::iterator i = files.begin(); i != files.end(); ++i) {
      char* name = ::bootes::lib::util::TChar::T2C(i->c_str());
      CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(name, id++, pdata);
      _pFileList->addItem(item);
      delete name;
   }

   return id;
}

void FileDialogLayout::dialog(const TCHAR* initdir, const TCHAR** exts)
{
   _exts.clear();
   for (const TCHAR** e=exts; *e != NULL && **e != _T('\0'); ++e) {
      _exts.push_back( std::basic_string< TCHAR >(*e) );
   }
   if (list(initdir) <= 0) { return; }
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
