#ifndef BOOTESDANCES_MOVE_MOVEMODELLINE_H
#define BOOTESDANCES_MOVE_MOVEMODELLINE_H

#include "../include.h"
#include "MoveModel.h"

class MoveModelLine: public MoveModel
{
public:
   MoveModelLine();
   MoveModelLine(const MoveModelLine&);
   virtual ~MoveModelLine();
   virtual MoveModel* clone() const { return new MoveModelLine(*this); }
   virtual inline TYPE getType() const { return T_LINE; }

   void init(float x0, float y0, float x1, float y1);
   void init(const t_points& points);
   /*
   bool set(const D3DXVECTOR3& beg, const D3DXVECTOR3& end);
   inline bool set(float x0, float y0, float x1, float y1) {
      return set(D3DXVECTOR3(x0,y0,0), D3DXVECTOR3(x1,y1,0)); }
   */
   bool setBegin(const D3DXVECTOR3& beg);
   bool setEnd(const D3DXVECTOR3& end);

   inline virtual bool canInsert(size_t) const { return true; }
   inline virtual bool canReplace(size_t) const { return true; }
   inline virtual bool canErase(size_t) const { return (2 < _edit_points.size()); }
   inline virtual bool canTransform() const { return true; }

   virtual bool replace(size_t i, const D3DXVECTOR3& p);
   virtual bool insert(size_t i, const D3DXVECTOR3& p);
   virtual bool erase(size_t i);
   virtual bool transform(const D3DXMATRIX& mat);

protected:
   void calcPlayPoints();
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
