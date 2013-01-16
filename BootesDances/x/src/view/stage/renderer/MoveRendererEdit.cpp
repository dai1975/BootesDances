#include "MoveRendererEdit.h"

MoveRendererEdit::MoveRendererEdit()
{
   clear();
}

MoveRendererEdit::~MoveRendererEdit()
{
}

bool MoveRendererEdit::init(IMoveEditor* editor)
{
   if (editor == NULL) { return false; }
   _pEditor = editor;
   return true;
}

void MoveRendererEdit::clear()
{
   _width = 0;
   _height = 0;
   _rendered.clear();
}

void MoveRendererEdit::onLostDevice()
{
}

void MoveRendererEdit::onResetDevice()
{
}

namespace {

bool sIsInSquare(float x, float y, const MoveRenderer::TriangleVertex* vtx)
{
   const MoveRenderer::TriangleVertex& a = vtx[0];
   const MoveRenderer::TriangleVertex& b = vtx[1];
   const MoveRenderer::TriangleVertex& c = vtx[3];
   const MoveRenderer::TriangleVertex& d = vtx[2];

   D3DXVECTOR3 ab(b.x-a.x, b.y-a.y, 0);
   D3DXVECTOR3 bc(c.x-b.x, c.y-b.y, 0);
   D3DXVECTOR3 cd(d.x-c.x, d.y-c.y, 0);
   D3DXVECTOR3 da(a.x-d.x, a.y-d.y, 0);

   D3DXVECTOR3 ap(x-a.x, y-a.y, 0);
   D3DXVECTOR3 bp(x-b.x, y-b.y, 0);
   D3DXVECTOR3 cp(x-c.x, y-c.y, 0);
   D3DXVECTOR3 dp(x-d.x, y-d.y, 0);

   float xa = ab.x * ap.y - ab.y * ap.x;
   float xb = bc.x * bp.y - bc.y * bp.x;
   float xc = cd.x * cp.y - cd.y * cp.x;
   float xd = da.x * dp.y - da.y * dp.x;

   if (0<xa && 0<xb && 0<xc && 0<xd) {
      return true;
   } else if (0>xa && 0>xb && 0>xc && 0>xd) {
      return true;
   } else {
      return false;
   }
}

};

void MoveRendererEdit::onRender(const Scene* scene, const MoveSequence* moves, double currentTime, int elapsedTime)
{
   IDirect3DDevice9* pDev = g_pFnd->getD3D9Device();
   __int64 t0 = scene->clock().clock;
   LONG w     = scene->videoinfo().width;
   LONG h     = scene->videoinfo().height;

   // 現時刻に表示するガイドを得る
   std::vector< const IMoveModel* > models;
   {
      MoveSequence::const_iterator i = moves->searchAt(t0, true);
      if (*i == NULL) { return; }
      MoveSequence::const_iterator i0 = i.group_begin();
      MoveSequence::const_iterator i1 = i.group_end();
      size_t n = 0;
      for (i=i0; i!=i1; ++i) { ++n; }
      models.resize(n);

      size_t a = 0;
      for (i=i0; i!=i1; ++i) {
         models[a++] = *i;
      }
   }

   ModelEditee edit_src, selected;
   IMoveModel* edit_tmp;
   bool isSelecting = _pEditor->editeeSelected(&selected);
   bool isEditing = _pEditor->editing(&edit_src, &edit_tmp);

   const std::vector< D3DXVECTOR3 >* points;
   _rendered.clear();
   _rendered.resize(models.size());
   for (size_t i=0; i<models.size(); ++i) {
      const MoveModel* model = static_cast< const MoveModel* >(models[i]);
      _rendered[i].model = model;
      {
         points = &model->getPlayPoints();
         DWORD color;
         BLEND_MODE blend = BLEND_DIFFUSEALPHA_TEXALPHA;
         if (isEditing && model == edit_src.model) {
            color = D3DCOLOR_ARGB(128, 0, 0, 0); //移動元は少し暗く
         } else if (isSelecting && model == selected.model) {
            color = D3DCOLOR_ARGB(128, 0, 255, 0);
         } else {
            color = D3DCOLOR_ARGB(0, 255, 255, 255);
         }
         pointsToTriangles(_rendered[i].ribbon_vtx, *points, _width, _height, color);
         drawTriangles(pDev, _rendered[i].ribbon_vtx, TEX_RIBBON, blend);
      }
      {
         points = &model->getEditPoints();
         DWORD color = D3DCOLOR_ARGB(255,0,0,0);
         pointsToPointList(_rendered[i].edit_vtx, *points, _width, _height, color);
         drawPointList(pDev, _rendered[i].edit_vtx, 10.0f, TEX_DOT);
      }
   }

   if (isEditing) {
      std::vector< TriangleVertex > vtx;
      points = &static_cast< MoveModel* >(edit_tmp)->getPlayPoints();
      DWORD color = D3DCOLOR_ARGB(128, 0, 0, 255);
      BLEND_MODE blend = BLEND_DIFFUSEALPHA_TEXALPHA;
      pointsToTriangles(vtx, *points, w, h, color);
      drawTriangles(pDev, vtx, TEX_RIBBON, blend);
   }

   _width = w;
   _height = h;
}

size_t MoveRendererEdit::getRenderedModels(std::vector<const IMoveModel*>* v) const
{
   size_t n = _rendered.size();
   if (v != NULL) {
      v->resize(n);
      for (size_t i=0; i<n; ++i) {
         v->at(i) = _rendered[i].model;
      }
   }
   return n;
}

bool MoveRendererEdit::presentLocated(float rx, float ry, ModelEditee* o) const
{
   float x = rx * _width;
   float y = ry * _height;
   t_rendered::const_iterator i;
   for (i=_rendered.begin(); i != _rendered.end(); ++i) {
      const std::vector< PointVertex >& vtx = i->edit_vtx;

      for (size_t j=0; j<vtx.size(); ++j) {
         if ((vtx[j].x - 10 <= x && x <= vtx[j].x + 10) &&
            (vtx[j].y - 10 <= y && y <= vtx[j].y + 10)) {
               o->model = i->model;
               o->joint = j;
               return true;
         }
      }
   }

   for (i=_rendered.begin(); i != _rendered.end(); ++i) {
      const std::vector< TriangleVertex >& vtx = i->ribbon_vtx;
      if (vtx.size() < 4) { continue; }

      for (size_t j=0; j<vtx.size()-2; j+=2) {
         if (sIsInSquare(x, y, &vtx[j])) {
            o->model = i->model;
            o->joint = -1;
            return true;
         }
      }
   }
   o->model = NULL;
   o->joint = -1;
   return false;
}

bool MoveRendererEdit::presentNearest(float rx, float ry, ModelEditee* o) const
{
   float x = rx * _width;
   float y = ry * _height;
   o->model = NULL;
   o->joint = -1;
   float min = 100.0f;

   t_rendered::const_iterator i;
   for (i=_rendered.begin(); i != _rendered.end(); ++i) {
      const std::vector< PointVertex >& vtx = i->edit_vtx;

      for (size_t j=0; j<vtx.size(); ++j) {
         float dx = vtx[j].x - x;
         float dy = vtx[j].y - y;
         float d = dx*dx + dy*dy;
         if (o->model == NULL || d < min) {
            o->model = i->model;
            o->joint = j;
            min = d;
         }
      }
   }
   return (o->model != NULL);
}

bool MoveRendererEdit::presentNearestEdge(float rx, float ry, ModelEditee* o) const
{
   float x = rx * _width;
   float y = ry * _height;
   o->model = NULL;
   o->joint = -1;
   float min = 100.0f;

   t_rendered::const_iterator i;
   for (i=_rendered.begin(); i != _rendered.end(); ++i) {
      const std::vector< PointVertex >& vtx = i->edit_vtx;

      for (size_t x=0; x<2; ++x) {
         size_t j = (x==0)? 0: vtx.size()-1;
         float dx = vtx[j].x - x;
         float dy = vtx[j].y - y;
         float d = dx*dx + dy*dy;
         if (o->model == NULL || d < min) {
            o->model = i->model;
            o->joint = j;
            min = d;
         }
      }
   }
   return (o->model != NULL);
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
