#include "MoveRenderer.h"

MoveRenderer::MoveRenderer()
{
   _resource = NULL;
}

MoveRenderer::~MoveRenderer()
{
}

void MoveRenderer::setResource(MoveRendererResource* p)
{
   _resource = p;
}

void MoveRenderer::pointsToTriangles(
       std::vector< TriangleVertex >& vtx,
       const std::vector< D3DXVECTOR3 >& points,
       LONG w, LONG h, DWORD color)
{
   if (points.size() < 2) { return; }
   vtx.resize((points.size() -1) * 4);

   float total_len = 0.0f;
   std::vector< float > len(points.size()-1);
   for (size_t i=0; i<points.size()-1; ++i) {
      const D3DXVECTOR3& p = points[i];
      const D3DXVECTOR3& q = points[i+1];
      float dx = (q.x - p.x);
      float dy = (q.y - p.y);
      len[i] = dx*dx + dy*dy;
      total_len += len[i];
   }

   float u = 0.0f;
   float thick = GetThickSize(h);
   for (size_t i=0; i<points.size()-1; ++i) {
      const D3DXVECTOR3& p = points[i];
      const D3DXVECTOR3& q = points[i+1];
      D3DXVECTOR3 crs;
      { //直線pqの-90度方向のベクトルcrsの計算。長さはthick.
         D3DXVECTOR3 tmp(p.y - q.y, q.x - p.x, 0);
         D3DXVec3Normalize(&crs, &tmp);
         crs *= thick;
      }
      float u0 = u;
      float u1 = u0 + (len[i] / total_len);
      if (1.0f < u1 || i==points.size()-2) { u1 = 1.0f; }
      u = u1; //次ループ用

      float px = p.x * w;
      float py = p.y * h;
      vtx[i*4+0].x = px + crs.x;
      vtx[i*4+0].y = py + crs.y;
      vtx[i*4+0].u = u0;
      vtx[i*4+0].v = 1.0f;

      vtx[i*4+1].x = px - crs.x;
      vtx[i*4+1].y = py - crs.y;
      vtx[i*4+1].u = u0;
      vtx[i*4+1].v = 0.0f;

      float qx = q.x * w;
      float qy = q.y * h;
      vtx[i*4+2].x = qx + crs.x;
      vtx[i*4+2].y = qy + crs.y;
      vtx[i*4+2].u = u1;
      vtx[i*4+2].v = 1.0f;

      vtx[i*4+3].x = qx - crs.x;
      vtx[i*4+3].y = qy - crs.y;
      vtx[i*4+3].u = u1;
      vtx[i*4+3].v = 0.0f;
   }

   for (size_t i=0; i<vtx.size(); ++i) {
      vtx[i].z = 0;
      vtx[i].rhw = 1.0f;
      vtx[i].color = color;
   }
}

HRESULT MoveRenderer::setBlendMode(IDirect3DDevice9* pDev, BLEND_MODE blend)
{
   //blend = BLEND_DEFAULT;
   switch (blend) {
   case BLEND_DIFFUSEALPHA_TEXALPHA:
      pDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_BLENDDIFFUSEALPHA );
      pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
      pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
      pDev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
      pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
      pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      pDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD );
      pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE );
      pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO );
      break;

   case BLEND_MODULATE_TEXALPHA:
      pDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
      pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
      pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
      pDev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
      pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
      pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      pDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD );
      pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
      pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
      break;

   default:
   case BLEND_DEFAULT:
      pDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
      pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
      pDev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
      pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
      pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
      pDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD );
      pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE );
      pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO );
      break;
   }
   return S_OK;
}

HRESULT MoveRenderer::drawTriangles(
          IDirect3DDevice9* pDev,
          const std::vector< TriangleVertex >& vtx,
          MoveRendererResource::TEXTURE_ID tex_id,
          BLEND_MODE blend)
{
   int nTriangles = vtx.size() - 2;
   if (nTriangles < 1) { return S_FALSE; }

   HRESULT hr = S_OK;

   size_t bufsize = sizeof(TriangleVertex) * vtx.size();
   IDirect3DVertexBuffer9* pVB;
   hr = pDev->CreateVertexBuffer(bufsize,
      D3DUSAGE_WRITEONLY, TRIANGLE_FVF, D3DPOOL_MANAGED, &pVB, NULL);
   if (FAILED(hr)) {
      return hr;
   }

   {
      void* p;
      hr = pVB->Lock(0, bufsize, &p, 0);
      if (FAILED(hr)) { goto fin; }
      memcpy(p, &vtx[0], bufsize);
      pVB->Unlock();
   }

   setBlendMode(pDev, blend);
   pDev->SetTexture(0, _resource->getTextureNoref(tex_id));
   pDev->SetFVF(TRIANGLE_FVF);
   pDev->SetStreamSource(0, pVB, 0, sizeof(TriangleVertex));
   pDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, nTriangles);
   hr = S_OK;
fin:
   pVB->Release();
   return hr;
}

void MoveRenderer::pointsToPointList(
       std::vector< PointVertex >& vtx,
       const std::vector< D3DXVECTOR3 >& points,
       LONG w, LONG h, DWORD color)
{
   if (points.size() < 1) { return; }
   vtx.resize(points.size());

   for (size_t i=0; i<points.size(); ++i) {
      const D3DXVECTOR3& p = points[i];
      float x,y;
      x = p.x * w;
      y = p.y * h;
      vtx[i].x = x;
      vtx[i].y = y;
      vtx[i].z = 0;
      vtx[i].z = 0;
      vtx[i].rhw = 1.0f;
      vtx[i].color = color;
   }
}

HRESULT MoveRenderer::drawPointList(
          IDirect3DDevice9* pDev,
          const std::vector< PointVertex >& vtx,
          float size,
          MoveRendererResource::TEXTURE_ID tex_id)
{
   int n = vtx.size();
   if (n < 1) { return S_FALSE; }

   HRESULT hr = S_OK;

   size_t bufsize = sizeof(PointVertex) * vtx.size();
   IDirect3DVertexBuffer9* pVB;
   hr = pDev->CreateVertexBuffer(bufsize,
      D3DUSAGE_WRITEONLY, POINT_FVF, D3DPOOL_MANAGED, &pVB, NULL);
   if (FAILED(hr)) {
      return hr;
   }

   {
      void* p;
      hr = pVB->Lock(0, bufsize, &p, 0);
      if (FAILED(hr)) { goto fin; }
      memcpy(p, &vtx[0], bufsize);
      pVB->Unlock();
   }
   pDev->SetStreamSource(0, pVB, 0, sizeof(PointVertex));
   pDev->SetFVF(POINT_FVF);
   pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
   pDev->SetRenderState(D3DRS_POINTSIZE, *(DWORD*)&size);
   pDev->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
   pDev->DrawPrimitive(D3DPT_POINTLIST, 0, n);

   pDev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
   hr = S_OK;
fin:
   pVB->Release();
   return hr;
}

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
