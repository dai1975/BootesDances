#include "GuideRibbon.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sstream>

GuideRibbon::GuideRibbon()
{
}

GuideRibbon::GuideRibbon(const GuideRibbon& r)
{
   _edit_points.resize( r._edit_points.size() );
   std::copy(r._edit_points.begin(), r._edit_points.end(),
      _edit_points.begin());

   _play_points.resize( r._play_points.size() );
   std::copy(r._play_points.begin(), r._play_points.end(),
      _play_points.begin());
}

GuideRibbon::~GuideRibbon()
{
}

const D3DXVECTOR3 GuideRibbon::getPlayPointAt(__int64 t) const
{
   const t_points& points = getPlayPoints();
   if (_pMove == NULL) { return points[0]; }

   __int64 t0 = _pMove->getBeginTime();
   __int64 t1 = _pMove->getEndTime();
   if (t <= t0) {
      return points[0];
   } else if (t1 <= t) {
      return points[points.size() -1];
   }
   float dt = (float)(t1 - t0) / 10000.0f;
   float rt = (float)(t  - t0) / 10000.0f;
   float ratio = rt / dt;

   float total_len = 0.0f;
   std::vector< float > length(points.size()-1);
   for (size_t i=0; i<points.size()-1; ++i) {
      D3DXVECTOR3 vp = points[i+1] - points[i];
      length[i] = D3DXVec3Length(&vp);
      total_len += length[i];
   }

   float rest_len = total_len * ratio;
   for (size_t i=0; i<points.size()-1; ++i) {
      if (length[i] < rest_len) {
         rest_len -= length[i];
         continue;
      } else {
         float r = rest_len / length[i];
         D3DXVECTOR3 vp = points[i+1] - points[i];
         vp *= r;
         D3DXVECTOR3 v = points[i] + vp;
         return v;
      }
   }

   return points[points.size() -1];
}

bool GuideRibbon::insert(size_t i, const D3DXVECTOR3& p)
{
   if (! canInsert(i)) { return false; }
   t_points::iterator ite = _edit_points.begin() + i;
   _edit_points.insert(ite, p);
   return true;
}
bool GuideRibbon::replace(size_t i, const D3DXVECTOR3& p)
{
   if (! canReplace(i)) { return false; }
   _edit_points[i] = p;
   return true;
}
bool GuideRibbon::erase(size_t i)
{
   if (! canErase(i)) { return false; }
   t_points::iterator ite = _edit_points.begin() + i;
   _edit_points.erase(ite);
   return true;
}
bool GuideRibbon::transform(const D3DXMATRIX& mat)
{
   if (! canTransform()) { return false; }

   std::vector< D3DXVECTOR4 > v(_edit_points.size());
   D3DXVec3TransformArray(
      &v[0], sizeof(D3DXVECTOR4),
      &_edit_points[0], sizeof(D3DXVECTOR3),
      &mat, _edit_points.size());
   for (size_t i=0; i<_edit_points.size(); ++i) {
      _edit_points[i].x = v[i].x;
      _edit_points[i].y = v[i].y;
   }
   return true;
}

bool GuideRibbon::translate(float dx, float dy)
{
   if (! canTransform()) { return false; }

   D3DXMATRIX mat;
   D3DXMatrixTranslation(&mat, dx, dy, 0.0f);
   return transform(mat);
}

void GuideRibbon::normalizeEditPoints() { }

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
