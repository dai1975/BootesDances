#ifndef BOOTESDANCES_IMOVEMODEL_H
#define BOOTESDANCES_IMOVEMODEL_H

#include <list>
#include <vector>
//#include "proto/Stage.pb.h"
#include <bootes/lib/booteslib.h> //D3DXVECTOR3
#include "IGuide.h"
#include "IMotion.h"

class IMove
{
public:
//   static IMove* Realize(const pb::Move*);
//   static pb::Move* Idealize(const IMove*);
/*
   enum TYPE {
      T_LINE, T_ELLIPSE, T_SPLINE,
   };
   virtual TYPE getType() const = 0;
*/
   virtual IMove* clone() const = 0;

   virtual const char* getUuid() const = 0;
   virtual void setUuid(const char*) = 0;
   virtual void genUuid() = 0;

   virtual __int64 getDuration() const = 0;
   virtual __int64 getBeginTime() const = 0;
   virtual __int64 getEndTime() const = 0;
   virtual void getTime(__int64* t0, __int64* t1) const = 0;
   virtual inline void setTime(__int64 t0, __int64 t1) = 0;

   virtual bool timeIn(__int64 t) const = 0;
   virtual bool timeOverlay(__int64 t0, __int64 t1) const = 0;
   virtual bool timeOverlay(const IMove* r) const = 0;

   virtual IGuide*  getGuide() = 0;
   virtual const IGuide*  getGuide() const = 0;
   virtual IMotion* getMotion() = 0;
   virtual const IMotion* getMotion() const = 0;
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
