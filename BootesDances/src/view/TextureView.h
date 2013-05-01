#ifndef BOOTESDANCES_VIEW_TEXTUREVIEW_H
#define BOOTESDANCES_VIEW_TEXTUREVIEW_H

#include "../include.h"

class TextureView
{
public:
   explicit TextureView();
   ~TextureView();

   inline bool isInit() const { return _init; }
   void reset();
   void init(D3DPOOL pool, DWORD width, DWORD height, D3DFORMAT format);
   IDirect3DTexture9* refTexture() const;

   void onLostDevice();
   void onResetDevice();
   bool beginScene(IDirect3DDevice9* pDev, IDirect3DSurface9** ppRet);
   void endScene(IDirect3DDevice9* pDev);

private:
   void clearBuffer();
   HRESULT createBuffer();

   bool _init;
   D3DPOOL _pool;
   DWORD _width, _height;
   D3DFORMAT _format;
   IDirect3DSurface9* _pRenderSurface;
   IDirect3DSurface9* _pDepthSurface;
   IDirect3DTexture9* _pExportTexture;
   IDirect3DSurface9* _pExportTmpSurface;
   IDirect3DSurface9 *_pBuf0, *_pDepth0;
};

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
