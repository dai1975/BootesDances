#include "MoveRealizer.h"
#include "Move.h"
#include "guide/GuideRealizer.h"
#include "motion/MotionRealizer.h"


bool MoveRealizer::Realize(IMove** ppOut, const pb::Move& in)
{
   IGuide* pGuide = NULL;
   IMotion* pMotion = NULL;
   if (! GuideRealizer::Realize(&pGuide, in.guide())) { return false; }
   if (in.has_motion()) {
      if (! MotionRealizer::Realize(&pMotion, in.motion())) { return false; }
   }

   Move* move = new Move();
   move->setGuide(pGuide);
   move->setMotion(pMotion);

   move->setTime(in.time0(), in.time1());
   move->setUuid(in.uuid().c_str());

   *ppOut = move;
   return true;
}

bool MoveRealizer::Idealize(::pb::Move* pOut, const IMove& in)
{
   if (in.getGuide()) {
      if (! GuideRealizer::Idealize(pOut->mutable_guide(), *in.getGuide())) {
         return false;
      }
   }
   if (in.getMotion()) {
      if (! MotionRealizer::Idealize(pOut->mutable_motion(), *in.getMotion())) {
         return false;
      }
   }

   pOut->set_uuid(in.getUuid());
   pOut->set_time0(in.getBeginTime());
   pOut->set_time1(in.getEndTime());

   return true;
}

bool MoveRealizer::IsExist(MotionGuidePair* pOut, const TCHAR* dir, const TCHAR* name, const MotionGuideList& mgl)
{
   for (MotionGuideList::const_iterator i = mgl.begin(); i != mgl.end(); ++i) {
      if (! MotionRealizer::IsExist(dir, name, i->motion.c_str())) { continue; }
      if (! GuideRealizer::IsExist(dir, name, i->guide.c_str())) { continue; }

      if (pOut != NULL) {
         pOut->motion = i->motion;
         pOut->guide = i->guide;
      }
      return true;
   }
   return false;
}

bool MoveRealizer::Save(const TCHAR* dir, const TCHAR* name, const MoveSequence& seq)
{
   if (! GuideRealizer::Save(dir, name, seq)) { return false; }
   if (! MotionRealizer::Save(dir, name, seq)) { return false; }
   return true;
}

bool MoveRealizer::Load(MoveSequence* seq, const TCHAR* dir, const TCHAR* name, const MotionGuidePair& mg)
{
   return false;
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
