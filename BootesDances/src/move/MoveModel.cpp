#include "MoveModel.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <sstream>

MoveModel::MoveModel()
{
   _t0 = 0;
   _t1 = 0;
   for (size_t i=0; i<Motion::NUM_DEVICES; ++i) {
      //_motion[i].input_mode = INPUT_TEST;
      //_motion[i].state = MOTION_DEFAULT;
      _motion[i].pMotion = NULL;
   }
   genUuid();
}

MoveModel::MoveModel(const MoveModel& r)
{
   _uuid = r._uuid;
   _t0 = r._t0;
   _t1 = r._t1;
   _edit_points.resize( r._edit_points.size() );
   std::copy(r._edit_points.begin(), r._edit_points.end(),
      _edit_points.begin());

   _play_points.resize( r._play_points.size() );
   std::copy(r._play_points.begin(), r._play_points.end(),
      _play_points.begin());

   for (size_t i=0; i<Motion::NUM_DEVICES; ++i) {
      //_motion[i].input_mode = r._motion[i].input_mode;
      //_motion[i].state      = r._motion[i].state;
      _motion[i].pMotion    = (r._motion[i].pMotion == NULL)? NULL: r._motion[i].pMotion->clone();
   }
}

MoveModel::~MoveModel()
{
   for (size_t i=0; i<Motion::NUM_DEVICES; ++i) {
      if (_motion[i].pMotion) {
         delete _motion[i].pMotion;
      }
   }
}

void MoveModel::setUuid(const char* uuid)
{
   _uuid = uuid;
}

void MoveModel::genUuid()
{
   ::boost::uuids::random_generator gen;
   ::boost::uuids::uuid uuid = gen();
   ::std::ostringstream os;
   os << uuid;
   _uuid = os.str();
}

void MoveModel::setTime(__int64 t0, __int64 t1)
{
   _t0 = t0;
   _t1 = t1;
   for (int i=0; i<Motion::NUM_DEVICES; ++i) {
      if (_motion[i].pMotion) {
         int dt = _t1 - _t0;
         _motion[i].pMotion->setDuration(dt);
      }
   }
}

Motion* MoveModel::getMotion(int dev) const
{
   return (dev < 0 || Motion::NUM_DEVICES <= dev)? NULL: _motion[dev].pMotion;
}

void MoveModel::setMotion(Motion* m)
{
   int dev = m->getDeviceType();
   if (dev < 0 || Motion::NUM_DEVICES <= dev) { return; }
   if (_motion[dev].pMotion) {
      delete _motion[dev].pMotion;
   }
   _motion[dev].pMotion = m;
}

const D3DXVECTOR3 MoveModel::getPlayPointAt(__int64 t) const
{
   __int64 t0 = getBeginTime();
   __int64 t1 = getEndTime();
   const t_points& points = getPlayPoints();
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

bool MoveModel::insert(size_t i, const D3DXVECTOR3& p)
{
   if (! canInsert(i)) { return false; }
   t_points::iterator ite = _edit_points.begin() + i;
   _edit_points.insert(ite, p);
   return true;
}
bool MoveModel::replace(size_t i, const D3DXVECTOR3& p)
{
   if (! canReplace(i)) { return false; }
   _edit_points[i] = p;
   return true;
}
bool MoveModel::erase(size_t i)
{
   if (! canErase(i)) { return false; }
   t_points::iterator ite = _edit_points.begin() + i;
   _edit_points.erase(ite);
   return true;
}
bool MoveModel::transform(const D3DXMATRIX& mat)
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

bool MoveModel::translate(float dx, float dy)
{
   if (! canTransform()) { return false; }

   D3DXMATRIX mat;
   D3DXMatrixTranslation(&mat, dx, dy, 0.0f);
   return transform(mat);
}

void MoveModel::normalizeEditPoints() { }

//---------------------------------------------------------------------------------
namespace {
   inline int GetDevice(const ::bootes::lib::framework::InputEvent* ev) {
      switch (ev->_type) {
      case ::bootes::lib::framework::InputEvent::T_WIIMOTE: return Motion::WIIMOTE;
      case ::bootes::lib::framework::InputEvent::T_KINECT: return Motion::KINECT;
      default: return -1;
      }
   }
}

MoveModel::MOTION_TEST_STATE MoveModel::getMotionTestState() const
{
   Motion::TEST_STATE s = Motion::TEST_DEFAULT;
   for (size_t i=0; i<Motion::NUM_DEVICES; ++i) {
      if (_motion[i].pMotion == NULL) { continue; }
      if (s < _motion[i].pMotion->getTestState()) {
         s = _motion[i].pMotion->getTestState();
      }
   }
   switch (s) {
   case Motion::TEST_DEFAULT: return MOTION_TEST_DEFAULT;
   case Motion::TEST_TESTING: return MOTION_TEST_TESTING;
   case Motion::TEST_SUCCEED: return MOTION_TEST_SUCCEED;
   case Motion::TEST_FAILED: return MOTION_TEST_FAILED;
   }
   return MOTION_TEST_DEFAULT;
}

MoveModel::MOTION_TEACH_STATE MoveModel::getMotionTeachState() const
{
   Motion::TEACH_STATE s = Motion::TEACH_DEFAULT;
   for (size_t i=0; i<Motion::NUM_DEVICES; ++i) {
      if (_motion[i].pMotion == NULL) { continue; }
      if (s < _motion[i].pMotion->getTeachState()) {
         s = _motion[i].pMotion->getTeachState();
      }
   }

   switch (s) {
   case Motion::TEACH_DEFAULT: return MOTION_TEACH_DEFAULT;
   case Motion::TEACH_TEACHING: return MOTION_TEACH_TEACHING;
   }
   return MOTION_TEACH_DEFAULT;
}

#if 0
void MoveModel::teachClear(const ::bootes::lib::framework::InputEvent* ev)
{
   int dev = GetDevice(ev);
   if (dev < 0 || Motion::NUM_DEVICES <= dev || _motion[dev].pMotion == NULL) { return; }
   _motion[dev].pMotion->teachClear();
}

void MoveModel::motionReady(const ::bootes::lib::framework::InputEvent* ev, INPUT_MODE mode)
{
   int dev = GetDevice(ev);
   if (dev < 0 || Motion::NUM_DEVICES <= dev || _motion[dev].pMotion == NULL) { return; }
   _motion[dev].state = (mode == INPUT_TEACH)? MOTION_TEACH: MOTION_DEFAULT;
}

void MoveModel::motionStart(const ::bootes::lib::framework::InputEvent* ev, INPUT_MODE mode)
{
   int dev = GetDevice(ev);
   if (dev < 0 || Motion::NUM_DEVICES <= dev || _motion[dev].pMotion == NULL) { return; }

   _motion[dev].input_mode = mode;
   _motion[dev].pMotion->setDuration(_t1 - _t0);
   switch (_motion[dev].input_mode) {
   case INPUT_TEST:
      _motion[dev].state = MOTION_DEFAULT;
      _motion[dev].pMotion->testBegin();
      break;
   case INPUT_TEACH:
      _motion[dev].state = MOTION_TEACH;
      _motion[dev].pMotion->teachBegin();
      break;
   }
}

bool MoveModel::motionInput(const ::bootes::lib::framework::InputEvent* ev, int t)
{
   int dev = GetDevice(ev);
   if (dev < 0 || Motion::NUM_DEVICES <= dev || _motion[dev].pMotion == NULL) { return false; }

   const ::bootes::lib::framework::WiimoteEvent* wev = static_cast< const ::bootes::lib::framework::WiimoteEvent* >(ev);
   __int64 rt = t - _t0;
   switch (_motion[dev].input_mode) {
   case INPUT_TEST:
      switch (_motion[dev].state) {
      case MOTION_DEFAULT:
      case MOTION_TEST:
         if (_motion[dev].pMotion->test(wev, rt)) {
            _motion[dev].state = MOTION_TEST;
         } else {
            _motion[dev].state = MOTION_FAILED;
         }
      }
      return (_motion[dev].state != MOTION_FAILED);

   case INPUT_TEACH:
      _motion[dev].state = MOTION_TEACH;
      return _motion[dev].pMotion->teach(wev, rt);
   }

   return false;
}

bool MoveModel::motionEnd(const ::bootes::lib::framework::InputEvent* ev, bool succeed)
{
   int dev = GetDevice(ev);
   if (dev < 0 || Motion::NUM_DEVICES <= dev || _motion[dev].pMotion == NULL) { return false; }

   switch (_motion[dev].input_mode) {
   case INPUT_TEST:
      if (_motion[dev].pMotion->testEnd(true)) { //end は現在状態に関わらず呼んでおく
         switch (_motion[dev].state) {
         case MOTION_DEFAULT:
         case MOTION_TEST:
            _motion[dev].state = MOTION_SUCCEED; break;
         default:
            _motion[dev].state = MOTION_FAILED; break;
         }
      } else {
         _motion[dev].state = MOTION_FAILED;
      }
      return (_motion[dev].state == MOTION_SUCCEED);

   case INPUT_TEACH:
      _motion[dev].state = MOTION_DEFAULT;
      return _motion[dev].pMotion->teachCommit(succeed);
   }
   return false;
}

void MoveModel::motionBreak(const ::bootes::lib::framework::InputEvent* ev)
{
   int dev = GetDevice(ev);
   if (dev < 0 || Motion::NUM_DEVICES <= dev || _motion[dev].pMotion == NULL) { return; }

   switch (_motion[dev].input_mode) {
   case INPUT_TEST:
      _motion[dev].pMotion->testEnd(false);
      _motion[dev].state = MOTION_FAILED;
      break;
   case INPUT_TEACH:
      _motion[dev].pMotion->teachRollback();
      _motion[dev].state = MOTION_DEFAULT;
      break;
   }
}
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
