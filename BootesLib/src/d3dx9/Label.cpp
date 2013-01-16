#include "../include.h"
#include <bootes/lib/d3dx9/Label.h>
#include <bootes/lib/d3dx9/Font.h>
#include <bootes/lib/booteslib.h>
#include "DrawTexture.h"

namespace bootes { namespace lib { namespace d3dx9 {

Label::Label()
{
}

Label::~Label()
{
}

void Label::setArea(float x, float y, float w, float h)
{
   m_x = x;
   m_y = y;
   m_w = w;
   m_h = h;
}

void Label::setCharSize(int s)
{
   m_char_size = s;
}

void Label::setText(const char* str)
{
   m_str = str;
}

void Label::draw()
{
   if (m_char_size < 1) { return; }

   size_t len = m_str.size();
   if (len < 1) { return; }

   IDirect3DDevice9* pDev = ::bootes::lib::GetFoundation()->getD3D9Device();
   ResourceView* res      = ::bootes::lib::GetFoundation()->getDxResource();

   Font font;
   if (! res->getFontTexture('m', &font)) {
      return;
   }
   float ratio = ((float)m_w) / (font.tm_max_w * m_char_size);

   float x = m_x;
   float w = 0;
   float y = m_y;
   float h;
   for (size_t i=0; i<len; ++i) {
      if (! res->getFontTexture(m_str[i], &font)) {
         continue;
      }
      x = x + w;
      w = font.gm_w * ratio;
      h = font.gm_h * ratio;

      DrawTexture(pDev, font.pTex, x, y, w, h);
      font.pTex->Release();
   }
}

} } }

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
