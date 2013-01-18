#include "MotionRealizer.h"
#include "MotionWiimoteSimple.h"

bool MotionRealizer::Idealize(::pb::Motion* pOut, const IMotion* pIn)
{
   return pIn->idealize(pOut);
}

bool MotionRealizer::Realize(IMotion** ppOut, const ::pb::Motion* pIn)
{
   if (pIn == NULL) {
      *ppOut = new MotionWiimoteSimple();
      return true;
   }
   IMotion* p = NULL;
   const std::string& type = pIn->type();

   if (false) {
      ;
   } else if (type.compare(MotionWiimoteSimple::TYPE) == 0) {
      p = new MotionWiimoteSimple();
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
