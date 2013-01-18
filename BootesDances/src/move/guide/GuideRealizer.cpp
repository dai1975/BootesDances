#include "GuideRealizer.h"
#include "GuideRibbonLine.h"
#include "GuideRibbonSpline.h"
#include "GuideRibbonEllipse.h"

bool GuideRealizer::Idealize(::pb::Guide* pOut, const IGuide* pIn)
{
   return pIn->idealize(pOut);
}

bool GuideRealizer::Realize(IGuide** ppOut, const ::pb::Guide* pIn)
{
   IGuide* p = NULL;
   const std::string& type = pIn->type();

   if (false) {
      ;
   } else if (type.compare(GuideRibbonLine::TYPE) == 0) {
      p = new GuideRibbonLine();
   } else if (type.compare(GuideRibbonEllipse::TYPE) == 0) {
      p = new GuideRibbonEllipse();
   } else if (type.compare(GuideRibbonSpline::TYPE) == 0) {
      p = new GuideRibbonSpline();
   } else {
      return false;
   }

   if (! p->realize(pIn)) {
      delete p;
      return false; 
   }
   
   *ppOut = p;
   return true;
}

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
