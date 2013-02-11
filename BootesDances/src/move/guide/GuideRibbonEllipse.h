#ifndef BOOTESDANCES_GUIDERIBBON_ELLIPSE_H
#define BOOTESDANCES_GUIDERIBBON_ELLIPSE_H

#include "../../include.h"
#include "GuideRibbon.h"

class GuideRibbonEllipse: public GuideRibbon
{
public:
   GuideRibbonEllipse();
   GuideRibbonEllipse(const GuideRibbonEllipse&);
   virtual ~GuideRibbonEllipse();
   virtual IGuide* clone() const { return new GuideRibbonEllipse(*this); }

   static const std::string TYPE;
   virtual bool idealize(::pb::Guide*) const;
   virtual bool realize(const ::pb::Guide&);
   virtual bool idealize(::pb::GuideRibbon* pOut) const;

   float getCenterX() const;
   float getCenterY() const;
   float getRadiusX() const;
   float getRadiusY() const;
   float getBeginAngle() const;
   float getEndAngle() const;
   bool getDirection() const;

   void init(float x, float y, float rx, float ry, float a0, float a1, bool dir);
   bool setCenter(float x, float y); //0..1
   bool setRadius(float rx, float ry); //0..1
   bool setBeginAngle(float radian);
   bool setEndAngle(float radian);
   bool setDirection(bool right);

   virtual bool canInsert(size_t i) const { return false; }
   virtual bool canReplace(size_t i) const { return true; }
   virtual bool canErase(size_t i) const { return false; }
   virtual bool canTransform() const { return true; }

   virtual bool replace(size_t i, const D3DXVECTOR3& v);
   virtual bool transform(const D3DXMATRIX& mat);
   virtual void normalizeEditPoints();

protected:
   void calcParameters();
   void calcPlayPoints();

private:
   enum JOINT_INDEX {
      JOINT_CENTER,
      JOINT_RADIUS,
      JOINT_BEGIN,
      JOINT_END,
      JOINT_DIRECTION,
      NUM_JOINTS,
   };

   float _x, _y;
   float _rx, _ry;
   float _arg0, _arg1;
   bool _dir;
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
