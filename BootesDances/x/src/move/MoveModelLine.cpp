#include "MoveModelLine.h"

MoveModelLine::MoveModelLine()
{
   _edit_points.resize(2);
   init(0.1f, 0.1f, 0.9f, 0.9f);
}

MoveModelLine::MoveModelLine(const MoveModelLine& r)
: MoveModel(r)
{
}

MoveModelLine::~MoveModelLine()
{
}

void MoveModelLine::init(float x0, float y0, float x1, float y1)
{
   _edit_points[0].x = x0;
   _edit_points[0].y = y0;
   _edit_points[0].z = 0.0f;
   _edit_points[1].x = x1;
   _edit_points[1].y = y1;
   _edit_points[1].z = 0.0f;
   calcPlayPoints();
}
void MoveModelLine::init(const t_points& points)
{
   _edit_points = points;
   calcPlayPoints();
}

bool MoveModelLine::setBegin(const D3DXVECTOR3& v)
{
   return replace(0, v);
}
bool MoveModelLine::setEnd(const D3DXVECTOR3& v)
{
   return replace(1, v);
}

bool MoveModelLine::replace(size_t i, const D3DXVECTOR3& v)
{
   if (! MoveModel::replace(i, v)) { return false; }
   _play_points[i] = _edit_points[i];
   return true;
}

bool MoveModelLine::transform(const D3DXMATRIX& mat)
{
   if (! MoveModel::transform(mat)) { return false; }
   calcPlayPoints();
   return true;
}

bool MoveModelLine::insert(size_t i, const D3DXVECTOR3& v)
{
   if (! MoveModel::insert(i, v)) { return false; }
   calcPlayPoints();
   return true;
}

bool MoveModelLine::erase(size_t i)
{
   if (! MoveModel::erase(i)) { return false; }
   calcPlayPoints();
   return true;
}

void MoveModelLine::calcPlayPoints()
{
   _play_points.resize( _edit_points.size() );
   std::copy(_edit_points.begin(),_edit_points.end(),_play_points.begin());
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
