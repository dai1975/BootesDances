#ifndef BOOTESDANCES_STAGEREALIZER_H
#define BOOTESDANCES_STAGEREALIZER_H

#include <bootes/dances/IMove.h>
#include "Stage.h"
#include <bootes/dances/proto/Stage.pb.h>

class StageRealizer
{
public:
   static bool Idealize(::pb::Stage*, const Stage*);
   static bool Realize(Stage**, const pb::Stage*);
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
