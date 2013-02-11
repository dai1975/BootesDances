#ifndef BOOTESDANCES_MOVE_MOVEREALIZER_H
#define BOOTESDANCES_MOVE_MOVEREALIZER_H

#include "../include.h"
#include <bootes/dances/IMove.h>
#include <bootes/dances/proto/Stage.pb.h>
#include "MoveSequence.h"
#include <map>
#include <list>

class MoveRealizer
{
public:
   static bool Idealize(::pb::Move*, const IMove&);
   static bool Realize(IMove**, const pb::Move&);

   struct MotionGuidePair {
      std::basic_string< TCHAR > motion;
      std::basic_string< TCHAR > guide;
      inline MotionGuidePair() { }
      inline MotionGuidePair(const TCHAR* m, const TCHAR* g): motion(m), guide(g) { }
      inline MotionGuidePair(const MotionGuidePair& r) { operator=(r); }
      inline MotionGuidePair& operator=(const MotionGuidePair& r) { motion = r.motion; guide = r.guide; return *this; }
   };
   typedef std::list< MotionGuidePair > MotionGuideList;
   static bool IsExist(MotionGuidePair* pOut, const TCHAR* dir, const TCHAR* name, const MotionGuideList&);
   static bool Load(MoveSequence* seq, const TCHAR* dir, const TCHAR* name, const MotionGuidePair&);
   static bool Save(const TCHAR* dir, const TCHAR* name, const MoveSequence& seq);
};

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
