#ifndef BOOTESDANCES_VIEW_STAGE_MOVERENDERERRESOURCE_H
#define BOOTESDANCES_VIEW_STAGE_MOVERENDERERRESOURCE_H

#include "../../../include.h"

class MoveRendererResourceEnum
{
public:
   enum TEXTURE_ID {
      TEX_RIBBON, TEX_DOT, TEX_MARKER,
      N_TEX,
   };
};

class MoveRendererResource: public MoveRendererResourceEnum
{
public:
   MoveRendererResource();
   ~MoveRendererResource();

   void onLostDevice();
   void onResetDevice();

   inline IDirect3DTexture9* getTextureNoref(TEXTURE_ID id) {
      return _tex[id];
   }

private:
   IDirect3DTexture9* _tex[N_TEX];
};

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
