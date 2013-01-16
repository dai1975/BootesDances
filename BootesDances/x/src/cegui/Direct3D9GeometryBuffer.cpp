#include <bootes/cegui/Direct3D9GeometryBuffer.h>
#include <iostream>
#include <algorithm>
#include <bootes/lib/d3dx9/Label.h>

namespace bootes { namespace cegui {

Direct3D9GeometryBuffer::Direct3D9GeometryBuffer(::CEGUI::Direct3D9Renderer& owner, LPDIRECT3DDEVICE9 device)
   : ::CEGUI::Direct3D9GeometryBuffer(owner, device)
{ }

void Direct3D9GeometryBuffer::reset()
{
   typedef ::CEGUI::Direct3D9GeometryBuffer super;
   super::reset();
   //m_vText.clear();
}

void Direct3D9GeometryBuffer::appendBoard(const CEGUI::Rect& pos, const CEGUI::Rect& tpos, const CEGUI::colour& color, CEGUI::Texture* tex)
{
   CEGUI::Vertex v[6];
   for (size_t i=0; i<6; ++i) {
      v[i].colour_val = color;
   }

   v[0].position   = CEGUI::Vector3( pos.d_left,   pos.d_bottom, 0);
   v[0].tex_coords = CEGUI::Vector2(tpos.d_left,  tpos.d_bottom);
   v[1].position   = CEGUI::Vector3( pos.d_left,   pos.d_top, 0);
   v[1].tex_coords = CEGUI::Vector2(tpos.d_left,  tpos.d_top);
   v[2].position   = CEGUI::Vector3( pos.d_right,  pos.d_bottom, 0);
   v[2].tex_coords = CEGUI::Vector2(tpos.d_right, tpos.d_bottom);
   v[3].position   = v[1].position;
   v[3].tex_coords = v[1].tex_coords;
   v[4].position   = v[2].position;
   v[4].tex_coords = v[2].tex_coords;
   v[5].position   = CEGUI::Vector3( pos.d_right,  pos.d_top, 0);
   v[5].tex_coords = CEGUI::Vector2(tpos.d_right, tpos.d_top);

   // GeometryBuffer では tex の ref は管理しない。毎回の描画で常に有効として扱う。
   setActiveTexture(tex);
   appendGeometry(&v[0], 6);
}

void Direct3D9GeometryBuffer::draw() const
{
   HRESULT hr;
   // 追加
   DWORD fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2;
   IDirect3DVertexBuffer9* pVB = NULL;
   if (0 < d_vertices.size()) {
      d_device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
      d_device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
      
      size_t bufsize = sizeof(D3DVertex) * d_vertices.size();
      hr = d_device->CreateVertexBuffer(bufsize, D3DUSAGE_WRITEONLY, fvf, D3DPOOL_MANAGED, &pVB, NULL);
      if (hr != S_OK || pVB == NULL) { return; }
      {
         void* p;
         pVB->Lock(0, bufsize, &p, 0);
         memcpy(p, &d_vertices[0], bufsize);
         pVB->Unlock();
      }
   }

#if 0
   typedef ::CEGUI::Direct3D9GeometryBuffer super;
   super::draw();
   if (pVB) { pVB->Release(); }
#else
   // setup clip region
   RECT clip;
   clip.left   = static_cast<LONG>(d_clipRect.d_left);
   clip.top    = static_cast<LONG>(d_clipRect.d_top);
   clip.right  = static_cast<LONG>(d_clipRect.d_right);
   clip.bottom = static_cast<LONG>(d_clipRect.d_bottom);
   d_device->SetScissorRect(&clip);
   
   // apply the transformations we need to use.
   if (!d_matrixValid) {
      updateMatrix();
   }
   
   d_device->SetTransform(D3DTS_WORLD, &d_matrix);
   
   d_owner.setupRenderingBlendMode(d_blendMode);
   
   const int pass_count = d_effect ? d_effect->getPassCount() : 1;
   for (int pass = 0; pass < pass_count; ++pass) {
      // set up RenderEffect
      if (d_effect)
         d_effect->performPreRenderFunctions(pass);
         
      // draw the batches
      size_t pos = 0;
      BatchList::const_iterator i = d_batches.begin();
      for ( ; i != d_batches.end(); ++i) {
         size_t nVerts = (*i).second;
         d_device->SetTexture(0, (*i).first);
         d_device->SetFVF(fvf);
         d_device->SetStreamSource(0, pVB, sizeof(D3DVertex)*pos, sizeof(D3DVertex));
         d_device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, nVerts / 3);
         // CEGUIDirect3D9GeometryBuffer で使っている DrawPrimitiveUp は遅い
               
         pos += nVerts;
      }
   }
   if (pVB) { pVB->Release(); }

   // clean up RenderEffect
   if (d_effect) {
      d_effect->performPostRenderFunctions();
   }
   d_device->SetTexture(0, NULL);
   d_device->SetFVF(fvf);
#endif
}
   
} }

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
