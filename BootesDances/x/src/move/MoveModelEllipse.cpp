#include "MoveModelEllipse.h"

MoveModelEllipse::MoveModelEllipse()
{
   _edit_points.resize(NUM_JOINTS);
   init(0.5f, 0.5f, 0.3f, 0.3f, -FLOAT_PI * 0.4f, -FLOAT_PI * 0.6f, true);
}

void MoveModelEllipse::init(float x, float y, float rx, float ry, float a0, float a1, bool dir)
{
   _x    = x;
   _y    = y;
   _rx   = rx;
   _ry   = ry;
   _arg0 = a0;
   _arg1 = a1;
   _dir  = dir;
   normalizeEditPoints();
   calcPlayPoints();
}

MoveModelEllipse::MoveModelEllipse(const MoveModelEllipse& r)
: MoveModel(r)
{
   _x = r._x;
   _y = r._y;
   _rx = r._rx;
   _ry = r._ry;
   _arg0 = r._arg0;
   _arg1 = r._arg1;
   _dir = r._dir;
}

MoveModelEllipse::~MoveModelEllipse()
{
}

float MoveModelEllipse::getCenterX() const
{
   return _x;
}
float MoveModelEllipse::getCenterY() const
{
   return _y;
}
float MoveModelEllipse::getRadiusX() const
{
   return _rx;
}
float MoveModelEllipse::getRadiusY() const
{
   return _ry;
}
float MoveModelEllipse::getBeginAngle() const
{
   return _arg0;
}
float MoveModelEllipse::getEndAngle() const
{
   return _arg1;
}
bool MoveModelEllipse::getDirection() const
{
   return _dir;
}

bool MoveModelEllipse::setCenter(float x, float y)
{
   D3DXVECTOR3 p(x,y,0);
   replace(JOINT_CENTER, p);
   return true;
}
bool MoveModelEllipse::setRadius(float rx, float ry)
{
   float y = _y + ry;
   D3DXVECTOR3 p(_x, y, 0); //updateで角度は補正されるので、距離だけセット
   replace(JOINT_RADIUS, p);
   return true;
}
bool MoveModelEllipse::setBeginAngle(float arg)
{
   float x = cos(arg) + _x;
   float y = sin(arg) + _y;
   D3DXVECTOR3 p(x,y,0);
   replace(JOINT_BEGIN, p);
   return true;
}
bool MoveModelEllipse::setEndAngle(float arg)
{
   float x = cos(arg) + _x;
   float y = sin(arg) + _y;
   D3DXVECTOR3 p(x,y,0);
   replace(JOINT_END, p);
   return true;
}
bool MoveModelEllipse::setDirection(bool d)
{
   if (_dir == d) { return true; }

   D3DXVECTOR3 p(0,0,0);
   if (d) {
      p.x = cos(_arg0 + 0.314f) + _x;
      p.y = sin(_arg0 + 0.314f) + _y;
   } else {
      p.x = cos(_arg0 - 0.314f) + _x;
      p.y = sin(_arg0 - 0.314f) + _y;
   }
   replace(JOINT_DIRECTION, p);
   return true;
}

bool MoveModelEllipse::replace(size_t idx, const D3DXVECTOR3& v)
{
   if (! MoveModel::replace(idx, v)) { return false; }

   switch (idx) {
      case JOINT_CENTER:
         calcParameters();
         return true;

      case JOINT_RADIUS:
         _rx = _edit_points[idx].x - _x;
         _ry = _edit_points[idx].y - _y;
         calcPlayPoints();
         return true;

      case JOINT_BEGIN: //atan2 returnes -PI..PI
         _arg0 = atan2(_edit_points[idx].y-_y, _edit_points[idx].x-_x);
         calcPlayPoints();
         return true;

      case JOINT_END:
         _arg1 = atan2(_edit_points[idx].y-_y, _edit_points[idx].x-_x);
         calcPlayPoints();
         return true;

      case JOINT_DIRECTION:
         {
            float a = atan2(_edit_points[idx].y-_y, _edit_points[idx].x-_x);
            if (_arg0 <= a) {
               _dir = (a < (_arg0 + FLOAT_PI));
            } else {
               _dir = (a < (_arg0 - FLOAT_PI));
            }
            calcPlayPoints();
         }
         return true;
   }
   return false;
}

bool MoveModelEllipse::transform(const D3DXMATRIX& mat)
{
   if (! MoveModel::transform(mat)) { return false; }

   calcParameters();
   return true;
}

// caluculate parameter by current edit points, then call calcPlayPoints()
void MoveModelEllipse::calcParameters()
{
   _x  = _edit_points[JOINT_CENTER].x;
   _y  = _edit_points[JOINT_CENTER].y;
   _rx = _edit_points[JOINT_RADIUS].x - _x;
   _ry = _edit_points[JOINT_RADIUS].y - _y;
   _arg0 = atan2(_edit_points[JOINT_BEGIN].y-_y, _edit_points[JOINT_BEGIN].x-_x);
   _arg1 = atan2(_edit_points[JOINT_END].y-_y, _edit_points[JOINT_END].x-_x);

   float a = atan2(_edit_points[JOINT_DIRECTION].y-_y, _edit_points[JOINT_DIRECTION].x-_x);
   if (_arg0 <= a) {
      _dir = (a < (_arg0 + FLOAT_PI));
   } else {
      _dir = (a < (_arg0 - FLOAT_PI));
   }
   calcPlayPoints();
}

// calc view points by current parameters
void MoveModelEllipse::calcPlayPoints()
{
   float rx = abs(_rx);
   float ry = abs(_ry);

   float step = FLOAT_PI/20; // temporary step
   size_t n = 0;
   float arg1;
   if (_dir) {
      arg1 = (_arg0 <= _arg1)? _arg1: _arg1 + 2*FLOAT_PI;
      for (float a=_arg0; a<arg1; a+=step) { ++n; }
      if (n == 0) { arg1 = _arg0 + step; }
   } else {
      arg1 = (_arg1 <= _arg0)? _arg1: _arg1 - 2*FLOAT_PI;
      for (float a=_arg0; a>arg1; a-=step) { ++n; }
      if (n == 0) { arg1 = _arg0 - step; }
   }

   _play_points.resize(n+1);
   step = (arg1 - _arg0) / n; // step size which is devided by number of steps
   float a = _arg0;
   for (size_t i=0; i<n+1; ++i) {
      _play_points[i].x = _x + rx*cos(a);
      _play_points[i].y = _y + ry*sin(a);
      a += step;
   }
}

// move edit points to normalized positions by current parameter
// because of parameters are not changed, view points are also no changed.
void MoveModelEllipse::normalizeEditPoints()
{
   float rx = abs(_rx);
   float ry = abs(_ry);
   _edit_points[JOINT_CENTER].x = _x;
   _edit_points[JOINT_CENTER].y = _y;
   _edit_points[JOINT_RADIUS].x = _x + _rx;
   _edit_points[JOINT_RADIUS].y = _y + _ry;
   _edit_points[JOINT_BEGIN].x = _x + rx*cos(_arg0);
   _edit_points[JOINT_BEGIN].y = _y + ry*sin(_arg0);
   _edit_points[JOINT_END].x = _x + rx*cos(_arg1);
   _edit_points[JOINT_END].y = _y + ry*sin(_arg1);

   if (_dir) {
      _edit_points[JOINT_DIRECTION].x = _x + rx*cos(_arg0 + 0.314f);
      _edit_points[JOINT_DIRECTION].y = _y + ry*sin(_arg0 + 0.314f);
   } else {
      _edit_points[JOINT_DIRECTION].x = _x + rx*cos(_arg0 - 0.314f);
      _edit_points[JOINT_DIRECTION].y = _y + ry*sin(_arg0 - 0.314f);
   }
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
