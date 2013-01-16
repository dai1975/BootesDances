#include <bootes/dances/BootesWiimote.h>
#include <bootes/lib/d3dx9.h>

using ::bootes::lib::framework::WiimoteEvent;

BootesWiimote::BootesWiimote()
{
   _tPaused = 0;
   _bCalib  = 0;
   ::memset(&_calib, 0, sizeof(_calib));
   ::memset(&_orien, 0, sizeof(_orien));
}

bool BootesWiimote::calcData(WiimoteEvent* ev1, const WiimoteEvent* ev0)
{
   if (! _bCalib) {
      if (ev1->isHeld1sec(WiimoteEvent::BTN_HOME)) {
         ::memset(&_calib, 0, sizeof(_calib));
         _tPaused = 0;
         _bCalib = true;
      }
   }

   DWORD dt =  (ev0->_gyro.t < ev1->_gyro.t)
      ? ev1->_gyro.t - ev0->_gyro.t
      : ev0->_gyro.t - ev1->_gyro.t;

   if (_bCalib) {
      D3DXVECTOR3 a(ev1->_accel.x, ev1->_accel.y, ev1->_accel.z);
      D3DXVECTOR3 g(ev1->_gyro.yaw, ev1->_gyro.pitch, ev1->_gyro.roll);
      if ((-0.1f <= a.x  && a.x < 0.1f) &&
          (-0.1f <= a.y  && a.y < 0.1f) &&
          ( 0.9f <= a.z  && a.z < 1.1f) &&
          (-20.0f <= g.x && g.x < 20.0f) &&
          (-20.0f <= g.y && g.y < 20.0f) &&
          (-20.0f <= g.z && g.z < 20.0f))
      {
         _nCalib++;
         _calib.x     += a.x;
         _calib.y     += a.y;
         _calib.z     += a.z - 1.0f;
         _calib.yaw   += g.x;
         _calib.pitch += g.y;
         _calib.roll  += g.z;

         _tPaused += dt;
         if (3000 < _tPaused) {
            _calib.x     /= _nCalib;
            _calib.y     /= _nCalib;
            _calib.z     /= _nCalib;
            _calib.yaw   /= _nCalib;
            _calib.pitch /= _nCalib;
            _calib.roll  /= _nCalib;
            ::memset(&_orien, 0, sizeof(_orien));
            _bCalib = false;
         }
      } else {
         _tPaused = 0;
         ::memset(&_calib, 0, sizeof(_calib));
         _nCalib = 0;
      }
   }
          
   if (! _bCalib) {
      ev1->_accel.x    -= _calib.x;
      ev1->_accel.y    -= _calib.y;
      ev1->_accel.z    -= _calib.z;
      ev1->_gyro.yaw   -= _calib.yaw;
      ev1->_gyro.pitch -= _calib.pitch;
      ev1->_gyro.roll  -= _calib.roll;
   }

   if (! filter(ev1)) { return false; }

   {
#define CUTOFF(PROP, LO, HI) if (LO < ev1->PROP && ev1->PROP < HI) { ev1->PROP = 0.0f; }

      CUTOFF(_gyro.yaw,   -10.0f,  10.0f);
      CUTOFF(_gyro.pitch, -10.0f,  10.0f);
      CUTOFF(_gyro.roll,  -10.0f,  10.0f);
#undef CUTOFF
   }   
   /*
   float       alen = D3DXVec3Length(&accel_vec);
   if (0.9f <= accel_len && accel_len <= 1.1f) {
         if (3000 < _tPaused) {
            D3DXVECTOR3 ori_vec /= accel_len;
            ev1->_orientation.x = ori_vec.x;
            ev1->_orientation.y = ori_vec.y;
            ev1->_orientation.z = ori_vec.z;
            ev1->_orientation.yaw   = 0;
            ev1->_orientation.pitch = 0;
            ev1->_orientation.roll  = 0;
            return;
         }
      } else {
         _tPaused = 0;
      }
   }
   */

#define INTEGRAL(PROP) { \
      float deg = dt/1000.0f * (ev0->_gyro.PROP + ev1->_gyro.PROP) / 2.0f; \
      deg += _orien.PROP;\
      while (180 < deg) deg -= 180; \
      while (deg < -180) deg += 180;\
      ev1->_orien.PROP = _orien.PROP = deg; \
}
   INTEGRAL(yaw);
   INTEGRAL(pitch);
   INTEGRAL(roll);
#undef INTEGRAL

   {
      D3DXMATRIX mat;
      D3DXVECTOR3 vec;
      float len;
      float yaw   = _orien.yaw * D3DX_PI / 180;
      float pitch = _orien.pitch * D3DX_PI / 180;
      float roll  = _orien.roll * D3DX_PI / 180;
      D3DXMatrixRotationYawPitchRoll(&mat, yaw, pitch, roll);
      vec = D3DXVECTOR3(mat._31, mat._32, mat._33); // to get rotated z-axis(0,0,1)
      len = D3DXVec3Length(&vec);
      if (0.01f < len) { vec /= len; }
      ev1->_orien.x = _orien.x = vec.x;
      ev1->_orien.y = _orien.y = vec.y;
      ev1->_orien.z = _orien.z = vec.z;
   }
   return true;
}

bool BootesWiimote::filter(WiimoteEvent* ev)
{
   float a = 0.5f;
   float b = 1 - a;

   WiimoteEvent last = _lpf;
   _lpf = *ev; //copy button, etc

#define LPF(NAME)\
   _lpf.NAME = b*last.NAME + a*ev->NAME;

   LPF(_accel.x);
   LPF(_accel.y);
   LPF(_accel.z);
   LPF(_gyro.yaw);
   LPF(_gyro.pitch);
   LPF(_gyro.roll);
#undef LPF
   *ev = _lpf;
   return true;
}


/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
