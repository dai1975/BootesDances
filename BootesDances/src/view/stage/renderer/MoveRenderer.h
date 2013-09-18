#ifndef BOOTESDANCES_VIEW_STAGE_MOVERENDERER_H
#define BOOTESDANCES_VIEW_STAGE_MOVERENDERER_H

#include "../../../include.h"
#include "MoveRendererResource.h"
#include "../../../move/MoveSequence.h"
#include <bootes/dances/IMove.h>
#include <bootes/dances/Scene.h>
#include <list>

class MoveRenderer: public MoveRendererResourceEnum
{
public:
   MoveRenderer();
   virtual ~MoveRenderer();
   void setResource(MoveRendererResource* p);

public:
   virtual void onLostDevice() = 0;
   virtual void onResetDevice()= 0;
   virtual void onRender(const ::bootes::lib::framework::GameTime* gt, const Scene* scene, const MoveSequence* moves) = 0;

public:
   static const DWORD TRIANGLE_FVF =
      ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
   struct TriangleVertex {
      float x,y,z;
      float rhw;
      DWORD color;
      float u,v;
   };

   static const DWORD POINT_FVF = 
      ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
   struct PointVertex {
      float x,y,z;
      float rhw;
      DWORD color;
      float u,v;
   };

protected:
   enum BLEND_MODE {
      BLEND_DEFAULT,
      BLEND_DIFFUSEALPHA_TEXALPHA,
      BLEND_MODULATE_TEXALPHA,
   };
   HRESULT setBlendMode(IDirect3DDevice9*, BLEND_MODE);

   inline static float GetThickSize(LONG h) { return h * 0.02f; }

   void pointsToTriangles(
      std::vector< TriangleVertex >& vtx,
      const std::vector< D3DXVECTOR3 >& points,
      LONG w, LONG h, DWORD color);

   HRESULT drawTriangles(
      IDirect3DDevice9*,
      const std::vector< TriangleVertex >& vtx,
      TEXTURE_ID tex,
      BLEND_MODE);

   void pointsToPointList(
      std::vector< PointVertex >& vtx,
      const std::vector< D3DXVECTOR3 >& points,
      LONG w, LONG h, DWORD color);

   HRESULT drawPointList(
      IDirect3DDevice9*,
      const std::vector< PointVertex >& vtx,
      float size,
      TEXTURE_ID tex);

protected:
   MoveRendererResource* _resource;
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
