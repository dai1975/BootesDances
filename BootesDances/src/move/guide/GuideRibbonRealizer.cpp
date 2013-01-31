#include "GuideRibbonRealizer.h"
#include "GuideRibbon.h"
#include "GuideRibbonLine.h"
#include "GuideRibbonEllipse.h"
#include "GuideRibbonSpline.h"
#include <google/protobuf/text_format.h>

namespace {

struct Static {
   inline Static() {
      assert(GuideFactory::Register(new GuideRibbonRealizer()));
   }
} _static;

}

const char* GuideRibbonRealizer::getGuideName() const
{
   return "GuideRibbon";
}

int GuideRibbonRealizer::countSubIds() const
{
   return GuideRibbon::NUM_SUBIDS;
}

IGuide* GuideRibbonRealizer::createGuide(int subid) const
{
   switch (subid) {
   case GuideRibbon::SUBID_LINE:    return new GuideRibbonLine();
   case GuideRibbon::SUBID_ELLIPSE: return new GuideRibbonEllipse();
   case GuideRibbon::SUBID_SPLINE:  return new GuideRibbonSpline();
   }
   assert(false);
   return NULL;
}

bool GuideRibbonRealizer::save(::google::protobuf::io::ZeroCopyOutputStream& out, const MoveSequence* seq) const
{
   ::pb::GuideRibbonList lst;
   for (MoveSequence::const_iterator i = seq->begin(); i != seq->end(); ++i) {
      ::pb::GuideRibbon* idea = lst.add_guides();
      const IMove* pMove = *i;
      const GuideRibbon* pGuide = static_cast< const GuideRibbon* >(pMove->getGuide());
      if (! pGuide->idealize(idea)) { return false; }

      idea->set_uuid(pMove->getUuid());
      idea->set_time0(pMove->getBeginTime());
      idea->set_time1(pMove->getEndTime());
      idea->set_chainnext( seq->isChainNext(i) );
   }
   if (! google::protobuf::TextFormat::Print(lst, &out)) { return false; }
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
