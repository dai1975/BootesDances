#ifndef BOOTESDANCES_MOVE_MOVEMODELSPLINE_H
#define BOOTESDANCES_MOVE_MOVEMODELSPLINE_H

#include "../include.h"
#include "MoveModel.h"

class MoveModelSpline: public MoveModel
{
public:
   MoveModelSpline();
   MoveModelSpline(const MoveModelSpline&);
   virtual ~MoveModelSpline();
   virtual inline MoveModel* clone() const { return new MoveModelSpline(*this); }
   virtual inline TYPE getType() const { return T_SPLINE; }

   void init(const t_points& points);

   bool setOnlinePoint(size_t i, float x, float y);
   bool setOfflinePoint(size_t i, float x, float y);

   virtual bool canInsert(size_t i) const { return true; }
   virtual bool canReplace(size_t i) const { return true; }
   virtual bool canTransform() const { return true; }
   virtual bool canErase(size_t i) const { return (3 < _edit_points.size()); }

   virtual bool insert(size_t i, const D3DXVECTOR3& v);
   virtual bool replace(size_t i, const D3DXVECTOR3& v);
   virtual bool erase(size_t i);
   virtual bool transform(const D3DXMATRIX& mat);

protected:
   void calcPlayPoints(size_t idx);
   void calcPlayPoints(size_t beg, size_t end);
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
