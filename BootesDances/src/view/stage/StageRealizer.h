#ifndef BOOTESDANCES_STAGEREALIZER_H
#define BOOTESDANCES_STAGEREALIZER_H

#include "../../include.h"
#include <bootes/dances/IMove.h>
#include <bootes/dances/Stage.h>
#include <bootes/dances/proto/Stage.pb.h>
#include <bootes/dances/proto/Teach.pb.h>
#include "../../move/MoveRealizer.h"
#include <list>

class StageRealizer
{
public:
   static bool Idealize(::pb::Stage*, const Stage&);
   static bool Realize(Stage**, const pb::Stage&, const TCHAR* name, const TCHAR* dirpath);

   typedef MoveRealizer::MotionGuidePair MotionGuidePair;
   typedef MoveRealizer::MotionGuideList MotionGuideList;
   static bool IsExist(const TCHAR* dir, const TCHAR* name);
   //static bool SetFactory(MoveSequence* pSeq, const MotionGuidePair& mg);

   static bool Search(void(*cb)(bool,Stage*,void*), void* data, const TCHAR* dir, const MotionGuideList&);
   static bool Save(std::basic_string<TCHAR>*, const TCHAR* dir, const TCHAR* name, bool neu, const Stage&, const MoveSequence&);
   static bool Load(Stage**, MoveSequence**, const TCHAR* dir, const TCHAR* name, const MotionGuideList&);
   static bool New(Stage**, MoveSequence**, const MotionGuidePair& mg);

private:
   static bool Search0(void(*cb)(bool,Stage*,void*), void* data, const TCHAR* basepath, const TCHAR* subdir, const MotionGuideList&);
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
