#include "MotionWiimoteSimpleRealizer.h"
#include "MotionWiimoteSimple.h"
#include <google/protobuf/text_format.h>

namespace {

struct Static {
   inline Static() {
      assert(MotionFactory::Register(new MotionWiimoteSimpleRealizer()));
   }
} _static;

}

const char* MotionWiimoteSimpleRealizer::getMotionName() const   { return "MotionWiimoteSimple"; }
const TCHAR* MotionWiimoteSimpleRealizer::getMotionNameT() const { return _T("MotionWiimoteSimple"); }
const char* MotionWiimoteSimpleRealizer::getGuideName() const    { return "GuideRibbon"; }
const TCHAR* MotionWiimoteSimpleRealizer::getGuideNameT() const  { return _T("GuideRibbon"); }

int MotionWiimoteSimpleRealizer::countSubIds() const
{
   return 1;
}

IMotion* MotionWiimoteSimpleRealizer::createMotion(int subid) const
{
   switch (subid) {
   case 0: return new MotionWiimoteSimple();
   }
   assert(false);
   return NULL;
}

bool MotionWiimoteSimpleRealizer::save(::google::protobuf::io::ZeroCopyOutputStream& out, const MoveSequence& seq) const
{
   ::pb::MotionWiimoteSimpleList lst;
   for (MoveSequence::const_iterator i = seq.begin(); i != seq.end(); ++i) {
      ::pb::MotionWiimoteSimple2* idea = lst.add_motions();
      const IMove* pMove = *i;
      const MotionWiimoteSimple* pMotion = static_cast< const MotionWiimoteSimple* >(pMove->getMotion());
      if (! pMotion->idealize(idea)) { return false; }

      idea->set_uuid(pMove->getUuid());
      idea->set_time0(pMove->getBeginTime());
      idea->set_time1(pMove->getEndTime());
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
