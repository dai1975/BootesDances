#ifndef BOOTESDANCES_VIEW_STAGE_MOVEVIEWRENDERERPLAY_H
#define BOOTESDANCES_VIEW_STAGE_MOVEVIEWRENDERERPLAY_H

#include "../../../include.h"
#include "MoveRenderer.h"
#include <vector>

class MoveRendererPlay: public MoveRenderer
{
public:
   MoveRendererPlay();
   virtual ~MoveRendererPlay();

   virtual bool init();
   virtual void clear();

public:
   virtual void onLostDevice();
   virtual void onResetDevice();
   virtual void onRender(const ::bootes::lib::framework::GameTime* gt, const Scene* scene, const MoveSequence* moves);

protected:
   void scaleVertex(std::vector< TriangleVertex >& vtx, const D3DXVECTOR3& center, float scale);
   void slideVertex(std::vector< TriangleVertex >& vtx, const D3DXVECTOR3& vtxcenter, LONG w, LONG h, float scale);
   void getMarkerTriangles(
      std::vector< TriangleVertex >& vtx,
      const D3DXVECTOR3 point,
      LONG w, LONG h, DWORD color);
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
