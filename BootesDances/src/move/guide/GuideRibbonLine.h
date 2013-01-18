#ifndef BOOTESDANCES_GUIDERIBBON_LINE_H
#define BOOTESDANCES_GUIDERIBBON_LINE_H

#include "../../include.h"
#include "GuideRibbon.h"
#include <string>

class GuideRibbonLine: public GuideRibbon
{
public:
   GuideRibbonLine();
   GuideRibbonLine(const GuideRibbonLine&);
   virtual ~GuideRibbonLine();
   virtual IGuide* clone() const { return new GuideRibbonLine(*this); }

   static const std::string TYPE;
   virtual bool idealize(::pb::Guide*) const;
   //virtual inline TYPE getType() const { return T_LINE; }

   void init(float x0, float y0, float x1, float y1);
   void init(const t_points& points);
   bool setBegin(const D3DXVECTOR3& beg);
   bool setEnd(const D3DXVECTOR3& end);

   inline virtual bool canInsert(size_t) const { return true; }
   inline virtual bool canReplace(size_t) const { return true; }
   inline virtual bool canErase(size_t) const { return (2 < _edit_points.size()); }
   inline virtual bool canTransform() const { return true; }

   virtual bool insert(size_t i, const D3DXVECTOR3& p);
   virtual bool replace(size_t i, const D3DXVECTOR3& p);
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
