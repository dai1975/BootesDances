#ifndef BOOTESDANCES_MOVE_MOVEMODEL_H
#define BOOTESDANCES_MOVE_MOVEMODEL_H

#include "../include.h"
#include <bootes/dances/IMoveModel.h>
#include "../motion/Motion.h"
#include <list>
#include <vector>

/**
様々なガイド形状の基底クラス。
画面の四隅を[0,0]-[1,1]とした座標軸を使う。
*/
class MoveModel: public IMoveModel
{
protected:
   MoveModel();
   MoveModel(const MoveModel&);
public:
   virtual ~MoveModel();

   virtual inline __int64 getBeginTime() const { return _t0; }
   virtual inline __int64 getEndTime() const { return _t1; }
   virtual inline void getTime(__int64* t0, __int64* t1) const {
      *t0 = _t0; *t1 = _t1;
   }
   virtual void setTime(__int64 t0, __int64 t1);

   virtual inline bool timeIn(__int64 t) const {
      return _t0 <= t && t < _t1;
   }
   virtual inline bool timeOverlay(__int64 t0, __int64 t1) const {
      return (t0 < _t1 && _t0 < t1);
   }
   virtual inline bool timeOverlay(const IMoveModel* r0) const {
      const MoveModel* r = static_cast< const MoveModel* >(r0);
      return timeOverlay(r->_t0, r->_t1);
   }

public:
   //! 編集用の点を得る
   virtual inline const t_points& getEditPoints() const
   { return _edit_points; }

   //! 表示用線分の点を得る
   inline const t_points& getPlayPoints() const
   { return _play_points; }

   //! 指定時刻におけるマーカーの位置を得る
   const D3DXVECTOR3 getPlayPointAt(__int64 t) const;

   virtual bool canInsert(size_t index) const  = 0;
   virtual bool canReplace(size_t index) const = 0;
   virtual bool canErase(size_t index) const   = 0;
   virtual bool canTransform() const           = 0;

   virtual bool insert(size_t index, const D3DXVECTOR3& p);
   virtual bool replace(size_t index, const D3DXVECTOR3& p);
   virtual bool erase(size_t index);
   virtual bool transform(const D3DXMATRIX& mat);

   inline bool insert(size_t index, float x, float y) {
      return insert(index, D3DXVECTOR3(x,y,0));
   }
   virtual inline bool replace(size_t index, float x, float y) {
      return replace(index, D3DXVECTOR3(x,y,0));
   }
   virtual bool translate(float dx, float dy);

   //! move editing points
   virtual void normalizeEditPoints();

   //-----------------------------------------------------------
   Motion* getMotion(int dev) const;
   void setMotion(Motion*);
/*
   virtual void teachClear(const ::bootes::lib::framework::InputEvent* ev);
   virtual void motionReady(const ::bootes::lib::framework::InputEvent* ev, INPUT_MODE m);
   virtual void motionStart(const ::bootes::lib::framework::InputEvent* ev, INPUT_MODE m);
   virtual bool motionInput(const ::bootes::lib::framework::InputEvent* ev, int t);
   virtual bool motionEnd(const ::bootes::lib::framework::InputEvent* ev, bool succeed);
   virtual void motionBreak(const ::bootes::lib::framework::InputEvent* ev);
*/

   enum MOTION_STATE { MOTION_DEFAULT, MOTION_TEACH, MOTION_TEST, MOTION_SUCCEED, MOTION_FAILED, };
   MOTION_STATE getMotionState() const;
protected:
   __int64 _t0, _t1;
   t_points _edit_points, _play_points;

   struct MotionEntry {
      //INPUT_MODE   input_mode;
      //MOTION_STATE state;
      Motion* pMotion;
   };
   MotionEntry _motion[Motion::NUM_DEVICES];
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
