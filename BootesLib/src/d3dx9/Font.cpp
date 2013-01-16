#include "../include.h"
#include <bootes/lib/d3dx9/Font.h>
#include <math.h>
#include <vector>

namespace bootes { namespace lib { namespace d3dx9 {

bool CreateFontTexture
(
 IDirect3DDevice9* pDev,
 DWORD usage,
 D3DPOOL pool,
 UINT ch,
 Font* pFont
) {
   HDC hdc = GetDC(NULL);
   HFONT hFont = ::CreateFont(0, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("MS UI Gothic")); 
   HFONT hOldFont = (HFONT)::SelectObject(hdc, hFont); 

   HRESULT hr;
   int gm_w, gm_h;
   int fnt_x, fnt_y;
   BYTE *bmp_p = NULL;
   int bmp_w, bmp_h; //bmp_p
   {
      TEXTMETRIC tm;
      GLYPHMETRICS gm;
      MAT2 mat2 = { {0,1}, {0,0}, {0,0}, {0,1} };

      GetTextMetrics(hdc, &tm);
      DWORD bufsize = GetGlyphOutline(
         hdc, ch, GGO_GRAY4_BITMAP, &gm, 0, NULL, &mat2);
      if (bufsize == GDI_ERROR) {
         bufsize = GetLastError();
         goto fin;
      }
      bmp_p = (BYTE*)malloc(bufsize);
      DWORD r = GetGlyphOutline(
         hdc, ch, GGO_GRAY4_BITMAP, &gm, bufsize, bmp_p, &mat2);

      pFont->tm_max_w = tm.tmMaxCharWidth;
      pFont->tm_ave_w = tm.tmAveCharWidth;
      gm_w = gm.gmCellIncX;
      gm_h = tm.tmHeight;
      bmp_w = ((gm.gmBlackBoxX + 3) / 4) * 4; //4-align
      bmp_h = gm.gmBlackBoxY;
      fnt_x = gm.gmptGlyphOrigin.x;
      fnt_y = tm.tmAscent - gm.gmptGlyphOrigin.y;
   }

   IDirect3DTexture9* pTex;
   hr = pDev->CreateTexture(gm_w, gm_h, 1,
      usage, D3DFMT_A8R8G8B8, pool,
      &pTex, NULL);

   {
      D3DLOCKED_RECT rect;
      pTex->LockRect(0, &rect, NULL, D3DLOCK_DISCARD);
      FillMemory(rect.pBits, rect.Pitch * gm_h, 0);
      for (int y=0; y<bmp_h; ++y) {
         BYTE* p = ((BYTE*)rect.pBits) + rect.Pitch * (fnt_y + y) + fnt_x * 4;
         for (int x=0; x<bmp_w; ++x) {
            DWORD trans = ((255 * bmp_p[x+y*bmp_w]) /16)&0xFF;
            DWORD color = 0x00FFFFFF | (trans << 24);
            memcpy(p, &color, 4);
            p += 4;
         }
      }
      pTex->UnlockRect(0);
   }
   pFont->pTex = pTex;
   pFont->gm_w = gm_w;
   pFont->gm_h = gm_h;
   pFont->fnt_x = fnt_x;
   pFont->fnt_y = fnt_y;

fin:
   if (bmp_p) { free(bmp_p); }
   SelectObject(hdc, hOldFont); 
   return true;
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
