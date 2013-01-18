#include "GuideRealizer.h"

bool GuideRealizer::Idealize(::pb::Guide* pOut, const IGuide* pIn)
{
   return pIn->idealize(pOut);
}

bool GuideRealizer::Realize(IGuide** ppOut, const ::pb::Guide* pIn)
{
   const std::string& type = pIn->type();
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
