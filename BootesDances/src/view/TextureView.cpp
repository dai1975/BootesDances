#include "TextureView.h"
#include <math.h>

TextureView::TextureView()
{
   _init = false;
   _pool = D3DPOOL_SYSTEMMEM;
   _width  = 64;
   _height = 64;
   _format = D3DFMT_X8R8G8B8;
   _pRenderSurface = NULL;
   _pDepthSurface = NULL;
   _pExportTmpSurface = NULL;
   _pExportTexture = NULL;
}

TextureView::~TextureView()
{
   clearBuffer();
}

void TextureView::reset()
{
   _init = false;
}

void TextureView::init(D3DPOOL pool, DWORD width, DWORD height, D3DFORMAT format)
{
   if (_init) { return; }
   _init = true;
   _pool = pool;
   _width = width;
   _height = height;
   _format = format;
   clearBuffer();
}

IDirect3DTexture9* TextureView::refTexture() const
{
   if (_pExportTexture == NULL) { return NULL; }
   _pExportTexture->AddRef();
   return _pExportTexture;
}

void TextureView::onResetDevice()
{
   IDirect3DDevice9* pDev = g_pFnd->getD3D9Device();
   clearBuffer();
}

void TextureView::onLostDevice()
{
   clearBuffer();
}

void TextureView::clearBuffer()
{
#define CLEAR(ptr) if (ptr) { ptr->Release(); ptr = NULL; }
   CLEAR(_pRenderSurface);
   CLEAR(_pDepthSurface);
   CLEAR(_pExportTexture);
   CLEAR(_pExportTmpSurface);
#undef CLEAR
}

HRESULT TextureView::createBuffer()
{
   if (_pExportTexture != NULL) { return S_OK; }
   clearBuffer();

   IDirect3DDevice9* pDev = g_pFnd->getD3D9Device();
   HRESULT hr;
   D3DSURFACE_DESC d;

   //CreateRenderTarget creates D3DPOOL_DEFAULT surface
   hr = pDev->CreateRenderTarget(
      _width, _height, _format, D3DMULTISAMPLE_NONE, 0, 
      true, &_pRenderSurface, NULL);
   if (FAILED(hr)) { goto fail; }
   _pRenderSurface->GetDesc(&d);

   // 深度ステンシルバッファの作成
   hr = pDev->CreateDepthStencilSurface(
      d.Width, d.Height, D3DFMT_D16,
      D3DMULTISAMPLE_NONE, 0, false, &_pDepthSurface, NULL);
   if (FAILED(hr)) { goto fail; }

   // CeguiTextureImage 提供用テクスチャの作成
   hr = pDev->CreateTexture(
      d.Width, d.Height, 1, 0, d.Format,
      _pool, &_pExportTexture, NULL);
   if (FAILED(hr)) { goto fail; }

   // create temporary texture to copy DEFAULT -> MANAGED
   // there is no function to transfer DEFAULT -> MANAGED and memcpy is very slow.
   if (_pool == D3DPOOL_MANAGED) {
      hr = pDev->CreateOffscreenPlainSurface(
                   d.Width, d.Height, d.Format,
                   D3DPOOL_SYSTEMMEM, &_pExportTmpSurface, NULL);
      if (FAILED(hr)) { goto fail; }
   }

   return S_OK;

fail:
   clearBuffer();
   return hr;
}

bool TextureView::beginScene(IDirect3DDevice9* pDev, IDirect3DSurface9** ppRet)
{
   HRESULT hr;

   if (ppRet) { *ppRet = NULL; }

   if (_pExportTexture == NULL) {
      hr = createBuffer();
      if (FAILED(hr)) {
         return false;
      }
   }

   // 現在のレンダリングターゲットへの参照を退避
   {
      hr = pDev->GetRenderTarget(0, &_pBuf0);
      if (FAILED(hr)) { return false; }
      hr = pDev->GetDepthStencilSurface(&_pDepth0);
      if (FAILED(hr)) { _pBuf0->Release(); return false; }

      //レンダリングターゲットの切り替え
      hr = pDev->SetRenderTarget(0, _pRenderSurface);
      hr = pDev->SetDepthStencilSurface(_pDepthSurface);
   }

   pDev->BeginScene();
   if (ppRet) {
      _pRenderSurface->AddRef();
      *ppRet = _pRenderSurface;
   }
   return true;
}

void TextureView::endScene(IDirect3DDevice9* pDev)
{
   HRESULT hr;

   pDev->EndScene();

   // レンダリングターゲットの復帰
   {
      hr = pDev->SetRenderTarget(0, _pBuf0);
      hr = pDev->SetDepthStencilSurface(_pDepth0);
      _pBuf0->Release(); //Get時のカウンタをデクリメント
      _pDepth0->Release();
   }

   IDirect3DSurface9* pBufSurface = _pRenderSurface;

   // copy image from rendered surface to export surface
   if (_pool == D3DPOOL_MANAGED) {
      // because d3d cannot directly copy from DEFAULT to MANAGED,
      //   1. setup temporary surface which is on SYSTEMMEM
      //   2. copy from DEFAULT to SYSTEMMEM by GetRenderTargetData()
      //   3. copy from SYSTEMMEM to MANAGED by memcpy //this is a bit slow.
      do {
         D3DSURFACE_DESC d0, d1;

         hr = pDev->GetRenderTargetData(pBufSurface, _pExportTmpSurface);

         _pExportTmpSurface->GetDesc(&d0);
         _pExportTexture->GetLevelDesc(0, &d1);

         D3DLOCKED_RECT r0, r1;
         _pExportTmpSurface->LockRect(&r0, NULL, D3DLOCK_READONLY);
         _pExportTexture->LockRect(0, &r1, NULL, 0);
         {
            int u = r0.Pitch / d0.Width;
            int xbytes = u * d0.Width;
            BYTE* p0 = (BYTE*)r0.pBits;
            BYTE* p1 = (BYTE*)r1.pBits;
            for (LONG y=0; y<d0.Height; ++y) {
               ::memcpy(p1, p0, xbytes);
               p0 += r0.Pitch;
               p1 += r1.Pitch;
            }
         }
         _pExportTexture->UnlockRect(0);
         _pExportTmpSurface->UnlockRect();
      } while (0);

   } else if (_pool == D3DPOOL_SYSTEMMEM) {
      IDirect3DSurface9* pDst;
      _pExportTexture->GetSurfaceLevel(0, &pDst);
      hr = pDev->GetRenderTargetData(pBufSurface, pDst);
      pDst->Release();
   }
}

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
