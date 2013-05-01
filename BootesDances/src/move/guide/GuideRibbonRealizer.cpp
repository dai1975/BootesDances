#include "GuideRibbonRealizer.h"
#include "GuideRibbon.h"
#include "GuideRibbonLine.h"
#include "GuideRibbonEllipse.h"
#include "GuideRibbonSpline.h"
#include <bootes/lib/util/TChar.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

namespace {

struct Static {
   inline Static() {
      assert(GuideRealizer::Register(new GuideRibbonRealizer()));
   }
} _static;

}

const char* GuideRibbonRealizer::getGuideName() const
{
   return "GuideRibbon";
}
const TCHAR* GuideRibbonRealizer::getGuideNameT() const
{
   return _T("GuideRibbon");
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

bool GuideRibbonRealizer::load(std::list< GuideData >* pOut, int fd) const
{
   ::pb::GuideRibbonList iList;
   {
      google::protobuf::io::FileInputStream in(fd);
      in.SetCloseOnDelete(false);
      bool parsed = google::protobuf::TextFormat::Parse(&in, &iList);
      if (!parsed) { return false; }
   }

   for (int i=0; i<iList.guides_size(); ++i) {
      const ::pb::GuideRibbon& idea = iList.guides(i);
      IGuide* pGuide = NULL;
      if (idea.has_line()) {
         GuideRibbonLine* p = new GuideRibbonLine();
         if (! p->realize(idea)) { delete p; continue; }
         pGuide = p;
      } else if (idea.has_ellipse()) {
         GuideRibbonEllipse* p = new GuideRibbonEllipse();
         if (! p->realize(idea)) { delete p; continue; }
         pGuide = p;
      } else if (idea.has_spline()) {
         GuideRibbonSpline* p = new GuideRibbonSpline();
         if (! p->realize(idea)) { delete p; continue; }
         pGuide = p;
      } else {
         continue; // unknown shape
      }
      if (pGuide == NULL) { continue; }

      pOut->push_back(GuideData());
      GuideData& d = pOut->back();

      d.pGuide    = pGuide;
      d.uuid      = idea.uuid();
      d.t0        = idea.time0();
      d.t1        = idea.time1();
      d.chainnext = idea.chainnext();
   }
   return true;
}

bool GuideRibbonRealizer::save(int fd, const MoveSequence& seq) const
{
   ::pb::GuideRibbonList lst;
   for (MoveSequence::const_iterator i = seq.begin(); i != seq.end(); ++i) {
      ::pb::GuideRibbon* idea = lst.add_guides();
      const IMove* pMove = *i;
      const GuideRibbon* pGuide = static_cast< const GuideRibbon* >(pMove->getGuide());
      if (! pGuide->idealize(idea)) { return false; }

      idea->set_uuid(pMove->getUuid());
      idea->set_time0(pMove->getBeginTime());
      idea->set_time1(pMove->getEndTime());
      idea->set_chainnext( seq.isChainNext(i) );
   }

   {
      google::protobuf::io::FileOutputStream ou(fd);
      ou.SetCloseOnDelete(false);
      bool r = google::protobuf::TextFormat::Print(lst, &ou);
      if (!r) { return false; }
      ou.Flush();
   }
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
