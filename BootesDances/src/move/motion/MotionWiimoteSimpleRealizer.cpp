#include "MotionWiimoteSimpleRealizer.h"
#include "MotionWiimoteSimple.h"
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

namespace {

struct Static {
   inline Static() {
      assert(MotionRealizer::Register(new MotionWiimoteSimpleRealizer()));
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

bool MotionWiimoteSimpleRealizer::save(int fd, const MoveSequence& seq) const
{
   ::pb::MotionWiimoteSimpleList lst;
   for (MoveSequence::const_iterator i = seq.begin(); i != seq.end(); ++i) {
      ::pb::MotionWiimoteSimple* idea = lst.add_motions();
      const IMove* pMove = *i;
      const MotionWiimoteSimple* pMotion = static_cast< const MotionWiimoteSimple* >(pMove->getMotion());
      if (! pMotion->idealize(idea)) { return false; }

      idea->set_uuid(pMove->getUuid());
      idea->set_time0(pMove->getBeginTime());
      idea->set_time1(pMove->getEndTime());
   }

   {
      google::protobuf::io::FileOutputStream out(fd);
      out.SetCloseOnDelete(false);
      bool b = google::protobuf::TextFormat::Print(lst, &out);
      if (! b) { return false; }
      out.Flush();
   }
   return true;
}

bool MotionWiimoteSimpleRealizer::load(std::list< MotionData >* pOut, int fd) const
{
   ::pb::MotionWiimoteSimpleList iList;
   {
      google::protobuf::io::FileInputStream in(fd);
      in.SetCloseOnDelete(false);
      bool parsed = google::protobuf::TextFormat::Parse(&in, &iList);
      if (!parsed) { return false; }
   }

   for (int i=0; i<iList.motions_size(); ++i) {
      const ::pb::MotionWiimoteSimple& idea = iList.motions(i);
      IMotion* pMotion = NULL;
      {
         MotionWiimoteSimple* p = new MotionWiimoteSimple();
         if (! p->realize(idea)) { delete p; continue; }
         pMotion = p;
      }
      if (pMotion == NULL) { continue; }

      pOut->push_back(MotionData());
      MotionData& d = pOut->back();
      
      d.pMotion   = pMotion;
      d.uuid      = idea.uuid();
      d.t0        = idea.time0();
      d.t1        = idea.time1();
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
