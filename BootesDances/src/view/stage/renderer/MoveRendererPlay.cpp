#include "MoveRendererPlay.h"

MoveRendererPlay::MoveRendererPlay()
{
}

MoveRendererPlay::~MoveRendererPlay()
{
}

bool MoveRendererPlay::init()
{
   return true;
}

void MoveRendererPlay::clear()
{
}

void MoveRendererPlay::onLostDevice()
{
}

void MoveRendererPlay::onResetDevice()
{
}

namespace {

D3DXVECTOR3 GetCenter(LONG w, LONG h, std::vector< const std::vector< D3DXVECTOR3 >* >& vpoints)
{
   float l = vpoints.at(0)->at(0).x;
   float r = vpoints.at(0)->at(0).x;
   float t = vpoints.at(0)->at(0).y;
   float b = vpoints.at(0)->at(0).y;

   for (size_t i=0; i<vpoints.size(); ++i) {
      const std::vector< D3DXVECTOR3 >* points = vpoints[i];
      for (size_t j=0; j<points->size(); ++j) {
         const D3DXVECTOR3& p = points->at(j);
         if (p.x < r) { r = p.x; }
         if (l < p.x) { l = p.x; }
         if (p.y < b) { b = p.y; }
         if (t < p.y) { t = p.y; }
      }
   }
   return D3DXVECTOR3(w*(r+l)/2, h*(t+b)/2, 0);
}

}

void MoveRendererPlay::scaleVertex(std::vector< TriangleVertex >& vtx, const D3DXVECTOR3& center, float scale)
{
   if (scale < 0.95f || 1.05f < scale) {
      for (size_t i=0; i<vtx.size(); ++i) {
         vtx[i].x = (vtx[i].x - center.x) * scale + center.x;
         vtx[i].y = (vtx[i].y - center.y) * scale + center.y;
      }
   }
}

void MoveRendererPlay::getMarkerTriangles(
    std::vector< TriangleVertex >& vtx,
    const D3DXVECTOR3 point,
    LONG w, LONG h, DWORD color)
{
   vtx.resize(4);

   float thick = MoveRenderer::GetThickSize(h) * 2.0f;

   float x = w * point.x;
   float y = h * point.y;

   vtx[0].x = x - thick;
   vtx[0].y = y + thick;
   vtx[0].u = 0.0f;
   vtx[0].v = 1.0f;

   vtx[1].x = x - thick;
   vtx[1].y = y - thick;
   vtx[1].u = 0.0f;
   vtx[1].v = 0.0f;

   vtx[2].x = x + thick;
   vtx[2].y = y + thick;
   vtx[2].u = 1.0f;
   vtx[2].v = 1.0f;

   vtx[3].x = x + thick;
   vtx[3].y = y - thick;
   vtx[3].u = 1.0f;
   vtx[3].v = 0.0f;

   for (size_t i=0; i<vtx.size(); ++i) {
      vtx[i].z = 0;
      vtx[i].rhw = 1.0f;
      vtx[i].color = color;
   }
}

void MoveRendererPlay::onRender(const ::bootes::lib::framework::GameTime* gt, const Scene* scene, const MoveSequence* moves)
{
   IDirect3DDevice9* pDev = g_pFnd->getD3D9Device();

   // 現在時刻より前後に伸ばした範囲と重なるMoveの取得(複数)
   int dt_pre  = 50000000; //5sec
   int dt_post = 20000000; //2sec
   __int64 t  = scene->clock().clock;
   int width  = scene->videoinfo().width;
   int height = scene->videoinfo().height;

   MoveSequence::const_iterator i0,i1; //[i0,i1)
   {
      __int64 t0 = t - dt_post;
      __int64 t1 = t + dt_pre;
      std::pair< MoveSequence::const_iterator,
         MoveSequence::const_iterator > p;
      p = moves->search(t0, t1);
      if (p.first == moves->end()) { return; }
      // [p.first, p.second]

      i0 = p.first.group_begin();
      i1 = p.second.group_end();
      // [i0,i1)
  }

   for (MoveSequence::const_iterator i=i0; i!=i1; i=i.group_end()) {
      MoveSequence::const_iterator j0 = i;
      MoveSequence::const_iterator j1 = i.group_end();

      size_t n = 0;
      float scale = 1.0f;
      {
         const IMove* pMove0 = *j0;
         const IMove* pMove1;
         for (MoveSequence::const_iterator j=j0; j!=j1; ++j) {
            pMove1 = *j;
            ++n;
         }

         if (t < pMove0->getBeginTime()) {
            // 開始時刻前の Move は縮小表示
            float dt = (float)(pMove0->getBeginTime() - t);
            scale = 1.0f - (dt / dt_pre);
            if (scale < 0.0f) {
               scale = 0.0f;
            } else if (1.0f < scale) {
               scale = 1.0f;
            }

         } else if (pMove1->getEndTime() < t) {
            // 終了後の Move は拡大表示
            float dt = (float)(t - pMove1->getEndTime());
            scale = 1.0f + 2 * (dt / dt_post);
            if (scale < 1.0f) {
               scale = 1.0f;
            } else if (3.0f < scale) {
               scale = 3.0f;
            }
         }
      }

      D3DXVECTOR3 screen_center(width/2, height/2, 0);
      D3DXVECTOR3 move_center; // セットで扱うMoveModel群の中心点を求める
      {
         std::vector< const std::vector< D3DXVECTOR3 >* > vpoints(n);
         size_t vi = 0;
         for (MoveSequence::const_iterator j=j0; j!=j1; ++j) {
            const IGuide* pGuide = (*j)->getGuide();
            if (pGuide) {
               vpoints[vi++] = &pGuide->getPlayPoints();
            }
         }
         move_center = GetCenter(width, height, vpoints);
      }

      //描画
      for (MoveSequence::const_iterator j=j0; j!=j1; ++j) {
         const IMove*   pMove   = *j;
         const IGuide*  pGuide  = pMove->getGuide();
         const IMotion* pMotion = pMove->getMotion();
         std::vector< TriangleVertex > vtx;
         DWORD color;

         if (pGuide) { //モデル描画
            color = D3DCOLOR_ARGB(0,0,0,0);
            const std::vector< D3DXVECTOR3 >& points = pGuide->getPlayPoints();
            if (pMotion) {
               switch (pMotion->getTestState()) {
               case IMotion::TEST_FAILED:
                  color = D3DCOLOR_ARGB(128,255,0,0);
                  break;
               default: {
                  unsigned char a = (1.0f < scale)? 255: static_cast<int>((255 * scale));
                  color = D3DCOLOR_ARGB(255-a,0,0,0);
               } break;
               }
            }
            if (scale < 0.95f) {
               pointsToTriangles(vtx, points, width, height, color);
               scaleVertex(vtx, move_center, scale);
               drawTriangles(pDev, vtx, TEX_RIBBON, BLEND_DIFFUSEALPHA_TEXALPHA);
            } else if (1.05f < scale) {
               ; //scaleVertex(vtx, screen_center, scale);
            } else {
               pointsToTriangles(vtx, points, width, height, color);
               drawTriangles(pDev, vtx, TEX_RIBBON, BLEND_DIFFUSEALPHA_TEXALPHA);
            }
         }

         if (pGuide) { //マーカー描画
            color = D3DCOLOR_ARGB(255, 0, 255, 0);
            if (pMotion) {
               if (pMotion->getTeachState() == IMotion::TEACH_TEACHING) {
                  color = D3DCOLOR_ARGB(255, 0, 255, 255);
               } else if (pMotion->getTestState() == IMotion::TEST_FAILED) {
                  color = D3DCOLOR_ARGB(255, 255, 0, 0);
               } else if (pMotion->getTestState() == IMotion::TEST_SUCCEED) {
                  color = D3DCOLOR_ARGB(255, 255, 255, 0);
               }
            }
            D3DXVECTOR3 v = pGuide->getPlayPointAt(scene->clock().clock);
            getMarkerTriangles(vtx, v, width, height, color);
            if (scale < 0.95f) {
               scaleVertex(vtx, move_center, scale);
            } else if (1.05f < scale) {
               scaleVertex(vtx, screen_center, scale);
            }
            drawTriangles(pDev, vtx, TEX_MARKER, BLEND_MODULATE_TEXALPHA);
         }
      }
   };
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
