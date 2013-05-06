#include "MoveRealizer.h"
#include "Move.h"
#include "guide/GuideRealizer.h"
#include "motion/MotionRealizer.h"
#include <bootes/lib/util/TChar.h>

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
   const GuideRealizer*  pGuideRealizer  = seq.getGuideRealizer();
   const MotionRealizer* pMotionRealizer = seq.getMotionRealizer();
   if (pGuideRealizer == NULL) { return false; }
   if (pMotionRealizer == NULL) { return false; }

   if (! pGuideRealizer->save(dir, name, seq)) { return false; }
   if (! pMotionRealizer->save(dir, name, seq)) { return false; }
   return true;
}

bool MoveRealizer::Load(MoveSequence** ppSeq, const TCHAR* dir, const TCHAR* name, const MotionGuidePair& mg)
{
   std::list< GuideRealizer::GuideData > guides;
   std::list< MotionRealizer::MotionData > motions;
   MoveSequence* pSeq = NULL;

   const GuideRealizer* pGuideRealizer = NULL;
   const MotionRealizer* pMotionRealizer = NULL;
   {
      char* c;
      c = ::bootes::lib::util::TChar::T2C(mg.guide.c_str());
      pGuideRealizer = GuideRealizer::GetRealizer(c);
      delete[] c;

      c = ::bootes::lib::util::TChar::T2C(mg.motion.c_str());
      pMotionRealizer = MotionRealizer::GetRealizer(c);
      delete[] c;
   }
   if (pGuideRealizer == NULL) { return false; }
   if (pMotionRealizer == NULL) { return false; }

   if (! pGuideRealizer->load(&guides, dir, name)) { goto fail; }
   if (! pMotionRealizer->load(&motions, dir, name)) { goto fail; }

   pSeq = new MoveSequence();
   pSeq->setGuideRealizer(pGuideRealizer);
   pSeq->setMotionRealizer(pMotionRealizer);

   bool chain = false;
   for (std::list< GuideRealizer::GuideData >::iterator i = guides.begin(); i != guides.end(); ++i) {
      GuideRealizer::GuideData& d = *i;
      Move* pMove = NULL;
      Move* pMove0 = static_cast< Move* >(pSeq->getMove(d.uuid.c_str()));
      if (pMove0) {
         if (pMove0->getGuide()) {
            delete d.pGuide;
            continue;
         }
         pMove = pMove0;
      } else {
         pMove = new Move();
      }
      pMove->setGuide(d.pGuide);
      pMove->setTime(d.t0, d.t1);
      pMove->setUuid(d.uuid.c_str());
      if (pMove0 == NULL) {
         pSeq->add(pMove);
      }
      MoveSequence::iterator ite = pSeq->search(pMove);
      pSeq->chainPrev(ite, chain);
      chain = d.chainnext;
   }
   for (std::list< MotionRealizer::MotionData >::iterator i = motions.begin(); i != motions.end(); ++i) {
      MotionRealizer::MotionData& d = *i;
      Move* pMove = static_cast< Move* >(pSeq->getMove(d.uuid.c_str()));
      if (pMove == NULL || pMove->getMotion() != NULL) {
         delete d.pMotion;
         continue;
      }
      pMove->setMotion(d.pMotion);
   }

   *ppSeq = pSeq;
   return true;

fail:
   if (pSeq) { delete pSeq; }
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
