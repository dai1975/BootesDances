#include "MoveRealizer.h"
#include "Move.h"
#include "guide/GuideRealizer.h"
#include "motion/MotionRealizer.h"


bool MoveRealizer::Realize(IMove** ppOut, const pb::Move* pIn)
{
   IGuide* pGuide = NULL;
   IMotion* pMotion = NULL;
   if (! GuideRealizer::Realize(&pGuide, &pIn->guide())) { return false; }
   if (pIn->has_motion()) {
      if (! MotionRealizer::Realize(&pMotion, &pIn->motion())) { return false; }
   } else {
      if (! MotionRealizer::Realize(&pMotion, NULL)) { return false; }
   }

   Move* move = new Move();
   move->setGuide(pGuide);
   move->setMotion(pMotion);

   move->setTime(pIn->time0(), pIn->time1());
   move->setUuid(pIn->uuid().c_str());

   *ppOut = move;
   return true;
}

bool MoveRealizer::Idealize(::pb::Move* pOut, const IMove* pIn)
{
   if (pIn->getGuide()) {
      if (! GuideRealizer::Idealize(pOut->mutable_guide(), pIn->getGuide())) {
         return false;
      }
   }
   if (pIn->getMotion()) {
      if (! MotionRealizer::Idealize(pOut->mutable_motion(), pIn->getMotion())) {
         return false;
      }
   }

   pOut->set_uuid(pIn->getUuid());
   pOut->set_time0(pIn->getBeginTime());
   pOut->set_time1(pIn->getEndTime());

   return true;
}

bool MoveRealizer::Save(const TCHAR* dir, const TCHAR* name, const MoveSequence* seq)
{
   if (! GuideRealizer::Save(dir, name, seq)) { return false; }
   if (! MotionRealizer::Save(dir, name, seq)) { return false; }
   return true;
}

bool MoveRealizer::Load(MoveSequence* seq, const TCHAR* dir, const TCHAR* name)
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
