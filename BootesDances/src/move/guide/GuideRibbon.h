#ifndef BOOTESDANCES_GUIDE_GUIDERIBBON_H
#define BOOTESDANCES_GUIDE_GUIDERIBBON_H

#include "../../include.h"
#include "Guide.h"
#include "GuideRibbon.pb.h"
#include <list>
#include <vector>

class GuideRibbon: public Guide
{
protected:
   GuideRibbon(const GuideRibbon&);
   explicit GuideRibbon(int subid);
public:
   virtual ~GuideRibbon();
   enum { SUBID_LINE, SUBID_ELLIPSE, SUBID_SPLINE,
          NUM_SUBIDS, };
   virtual bool idealize(::pb::GuideRibbon* pOut) const = 0;

public:
   static const std::string TYPE;
   //! 編集用の点を得る
   virtual inline const t_points& getEditPoints() const
   { return _edit_points; }

   //! 表示用線分の点を得る
   virtual inline const t_points& getPlayPoints() const
   { return _play_points; }

   //! 指定時刻(Guide内相対)におけるマーカーの位置を得る
   virtual const D3DXVECTOR3 getPlayPointAt(__int64 t) const;

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

protected:

protected:
   t_points _edit_points, _play_points;
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
