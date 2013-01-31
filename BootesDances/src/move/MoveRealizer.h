#ifndef BOOTESDANCES_MOVE_MOVEREALIZER_H
#define BOOTESDANCES_MOVE_MOVEREALIZER_H

#include "../include.h"
#include <bootes/dances/IMove.h>
#include <bootes/dances/proto/Stage.pb.h>
#include "MoveSequence.h"

class MoveRealizer
{
public:
   static bool Idealize(::pb::Move*, const IMove*);
   static bool Realize(IMove**, const pb::Move*);

   static bool Save(const TCHAR* dir, const TCHAR* name, const MoveSequence* seq);
   static bool Load(MoveSequence* seq, const TCHAR* dir, const TCHAR* name);
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
