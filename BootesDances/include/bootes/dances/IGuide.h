#ifndef BOOTESDANCES_IGUIDE_H
#define BOOTESDANCES_IGUIDE_H

#include <list>
#include <vector>
#include "proto/Stage.pb.h"
#include <bootes/lib/booteslib.h> //D3DXVECTOR3

class IMove;
class IGuide
{
public:
   inline IGuide() { }
   inline virtual ~IGuide() { };
   virtual int getSubId() const = 0;
   virtual IGuide* clone() const = 0;

   virtual void setMove(IMove*) = 0;
   virtual bool idealize(::pb::Guide* pOut) const = 0;
   virtual bool realize(const ::pb::Guide& in) = 0;

   virtual bool canInsert(size_t index) const  = 0;
   virtual bool canReplace(size_t index) const = 0;
   virtual bool canErase(size_t index) const   = 0;
   virtual bool canTransform() const           = 0;

   virtual bool insert(size_t index, const D3DXVECTOR3& p) = 0;
   virtual bool replace(size_t index, const D3DXVECTOR3& p) = 0;
   virtual bool erase(size_t index) = 0;
   virtual bool transform(const D3DXMATRIX& mat) = 0;
/*
   inline bool insert(size_t index, float x, float y) {
      return insert(index, D3DXVECTOR3(x,y,0));
   }
   virtual inline bool replace(size_t index, float x, float y) {
      return replace(index, D3DXVECTOR3(x,y,0));
   }
*/
   virtual bool translate(float dx, float dy) = 0;

public:
   typedef std::vector< D3DXVECTOR3 > t_points;
   virtual inline const t_points& getEditPoints() const = 0;
   virtual void normalizeEditPoints() = 0;

   virtual const t_points& getPlayPoints() const = 0;
   virtual const D3DXVECTOR3 getPlayPointAt(__int64 t) const = 0;
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
