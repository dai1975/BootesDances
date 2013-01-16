#ifndef BOOTESDANCES_CEGUI_DYANAMICFONT_H
#define BOOTESDANCES_CEGUI_DYANAMICFONT_H

#include <CEGUI.h>
#include <string>

namespace bootes { namespace cegui {

class DynamicFont
{
public:
   static std::string DynamicFont::GetAsciiFontName(int size);
   static std::string DynamicFont::GetKanjiFontName(int size);

   static CEGUI::Font* GetAsciiFont(int size);
   static CEGUI::Font* GetKanjiFont(int size);
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
