#ifndef BOOTES_LIB_D3DX9_FONT_H
#define BOOTES_LIB_D3DX9_FONT_H

#include "macros.h"

namespace bootes { namespace lib { namespace d3dx9 {

struct Font
{
   IDirect3DTexture9* pTex;
   int tm_max_w;
   int tm_ave_w;
   int gm_w, gm_h;   //
   int fnt_x, fnt_y; //
};

bool CreateFontTexture(IDirect3DDevice9* pDev, DWORD usage, D3DPOOL pool, UINT ch, Font* out);

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
