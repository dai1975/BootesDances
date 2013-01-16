#include "../include.h"
#include <bootes/cegui/DynamicFont.h>
#include <iostream>

namespace bootes { namespace cegui {

std::string DynamicFont::GetAsciiFontName(int size)
{
   std::ostringstream o;
   o << "bootes-a" << size;
   return o.str();
}
std::string DynamicFont::GetKanjiFontName(int size)
{
   std::ostringstream o;
   o << "bootes-k" << size;
   return o.str();
}

namespace {
CEGUI::Font* GetFont(const std::string& fname, const char* filename, int size)
{
   CEGUI::Font* font = NULL;
   try {
      if (CEGUI::FontManager::getSingleton().isDefined(fname)) {
         font = &CEGUI::FontManager::getSingleton().get(fname);
      } else {
         font = &CEGUI::FontManager::getSingleton().createFreeTypeFont(
            fname, size, true, filename, "", true);
      }

   } catch (CEGUI::Exception& e) {
      const char* msg = e.getMessage().c_str();
      const char* filename = e.getFileName().c_str();
      const char* name = e.getName().c_str();
      int line = e.getLine();
      DebugBreak();
   }
   return font;
}
}

CEGUI::Font* DynamicFont::GetAsciiFont(int size)
{
   std::string fname = GetAsciiFontName(size);
   return GetFont(fname, "DejaVuSans.ttf", size);
}

CEGUI::Font* DynamicFont::GetKanjiFont(int size)
{
   std::string fname = GetAsciiFontName(size);
   return GetFont(fname, "ipamp.ttf", size);
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
