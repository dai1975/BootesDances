#ifndef BOOTES_LIB_D3DX9_LABEL_H
#define BOOTES_LIB_D3DX9_LABEL_H

#include "macros.h"
#include <string>

namespace bootes { namespace lib { namespace d3dx9 {

class Label
{
public:
   Label();
   virtual ~Label();

   void setArea(float x, float y, float w, float h);
   void setCharSize(int len);
   void setText(const char* str);
   void draw();

private:
   int m_char_size;
   float m_x, m_y, m_w, m_h;
   std::string m_str;
};

} } }

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
