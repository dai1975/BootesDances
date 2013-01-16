#ifndef BOOTES_LIB_SRC_D3DX9_DRAWTEXTURE_H
#define BOOTES_LIB_SRC_D3DX9_DRAWTEXTURE_H

#include <bootes/lib/d3dx9.h>

namespace bootes { namespace lib { namespace d3dx9 {

HRESULT DrawTexture(IDirect3DDevice9* pDev, IDirect3DTexture9* pTex,
                    float x0, float y0, float w, float h);

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
