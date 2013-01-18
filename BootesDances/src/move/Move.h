#ifndef BOOTESDANCES_MOVE_MOVE_H
#define BOOTESDANCES_MOVE_MOVE_H

#include "../include.h"
#include <bootes/dances/IMove.h>
#include "guide/Guide.h"
#include "motion/Motion.h"
#include <list>
#include <vector>

class Move: public IMove
{
   Move(const Move&);
public:
   Move();
   virtual ~Move();
   virtual IMove* clone() const;

   void setGuide(IGuide*);
   void setMotion(IMotion*);
   virtual IGuide*  getGuide();
   virtual IMotion* getMotion();
   virtual const IGuide*  getGuide() const;
   virtual const IMotion* getMotion() const;

public:
   virtual const char* getUuid() const { return _uuid.c_str(); }
   virtual void setUuid(const char*);
   virtual void genUuid();

   virtual inline __int64 getBeginTime() const { return _t0; }
   virtual inline __int64 getEndTime() const { return _t1; }
   virtual inline __int64 getDuration() const { return _t1 - _t0; }
   virtual inline void getTime(__int64* t0, __int64* t1) const { *t0 = _t0; *t1 = _t1; }
   virtual void setTime(__int64 t0, __int64 t1);

   virtual inline bool timeIn(__int64 t) const { return _t0 <= t && t < _t1; }
   virtual inline bool timeOverlay(__int64 t0, __int64 t1) const { return (t0 < _t1 && _t0 < t1); }
   virtual inline bool timeOverlay(const IMove* r0) const {
      const Move* r = static_cast< const Move* >(r0);
      return timeOverlay(r->_t0, r->_t1);
   }

protected:
   std::string _uuid;
   __int64 _t0, _t1;

   IGuide*  _pGuide;
   IMotion* _pMotion;
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
