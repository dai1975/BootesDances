#ifndef BOOTESDANCES_MOVE_MOVEREALIZER_H
#define BOOTESDANCES_MOVE_MOVEREALIZER_H

#include <bootes/dances/IMove.h>
#include <bootes/dances/proto/Stage.pb.h>

class MoveRealizer
{
public:
   static bool Idealize(::pb::Move2*, const IMove*);
   static bool Realize(IMove**, const pb::Move*);
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
