#ifndef BOOTESDANCES_MOVEEDITEE_H
#define BOOTESDANCES_MOVEEDITEE_H

#include "IMove.h"

struct MoveEditee
{
   const IMove* pMove;
   int joint;
   inline MoveEditee(): pMove(NULL), joint(-1) { }
   inline MoveEditee(const MoveEditee& r) { operator=(r); }
   inline MoveEditee& operator=(const MoveEditee& r) {
      pMove = r.pMove;
      joint = r.joint;
      return *this;
   }
};

#endif

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
