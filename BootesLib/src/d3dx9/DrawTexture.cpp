#include "../include.h"
#include "DrawTexture.h"
#include <bootes/lib/d3dx9.h>
#include <math.h>
#include <vector>

namespace bootes { namespace lib { namespace d3dx9 {

namespace {
   struct Vertex {
      float x,y,z,rhw;
      DWORD color;
      float u, v;

      inline static DWORD FVF() {
         return D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2;
      }
   };
}
HRESULT DrawTexture(
 IDirect3DDevice9* pDev,
 IDirect3DTexture9* pTex,
 float x0, float y0, float w, float h
) {
  HRESULT hr;

  float x1 = x0 + w;
  float y1 = y0 + h;
  std::vector< Vertex > vtx;
  {
     vtx.resize(4);
     for (size_t i=0; i<vtx.size(); ++i) {
        vtx[i].color = D3DCOLOR_ARGB(255,255,255,255);
        vtx[i].rhw = 1.0f;
        vtx[i].z = 0.0f;
     }
     vtx[0].x = x0; vtx[0].y = y0; vtx[0].u = 0.0f; vtx[0].v = 0.0f;
     vtx[1].x = x0; vtx[1].y = y1; vtx[1].u = 0.0f; vtx[1].v = 1.0f;
     vtx[2].x = x1; vtx[2].y = y0; vtx[2].u = 1.0f; vtx[2].v = 0.0f;
     vtx[3].x = x1; vtx[3].y = y1; vtx[3].u = 1.0f; vtx[3].v = 1.0f;
   }

  DWORD fvf = Vertex::FVF();

  IDirect3DVertexBuffer9* pVB;
  {
     size_t bufsize = sizeof(Vertex) * vtx.size();
     hr = pDev->CreateVertexBuffer(
        bufsize, D3DUSAGE_WRITEONLY, fvf, D3DPOOL_MANAGED, &pVB, NULL);
     if (hr != S_OK || pVB == NULL) {
        return hr;
     }

     void* p;
     pVB->Lock(0, bufsize, &p, 0);
     memcpy(p, &vtx[0], bufsize);
     pVB->Unlock();
  }

  {
      pDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
      pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
      pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
      pDev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
      pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
      pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      pDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD );
      pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
      pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
  }
  pDev->SetTexture(0, pTex);
  pDev->SetFVF(fvf);
  pDev->SetStreamSource(0, &pVB[0], 0, sizeof(Vertex));
  pDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

  pVB->Release();
  return S_OK;
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
