#ifndef BOOTESDANCES_STAGEREALIZER_H
#define BOOTESDANCES_STAGEREALIZER_H

#include "../../include.h"
#include <bootes/dances/IMove.h>
#include "Stage.h"
#include <bootes/dances/proto/Stage.pb.h>
#include <bootes/dances/proto/Teach.pb.h>

class StageRealizer
{
public:
   static bool Idealize(::pb::Stage*, const Stage*);
   static bool Realize(Stage**, const pb::Stage*);

   static bool Save(const TCHAR* dir, const TCHAR* name, const Stage*);
   static bool Load(Stage**, const TCHAR* path);
   static bool Load(::pb::Stage**, const TCHAR* path);
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
