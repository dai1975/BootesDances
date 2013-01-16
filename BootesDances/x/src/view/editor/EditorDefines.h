#ifndef BOOTESDANCES_VIEW_CEGUIEDITORDEFINES_H
#define BOOTESDANCES_VIEW_CEGUIEDITORDEFINES_H

#include "../../include.h"
#include <string>

#pragma warning(disable : 4995)
#include <CEGUI.h>

namespace bootes { namespace dances { namespace editor {

struct EditPen {
   enum PenType {
      PEN_ACTION,
      PEN_NEWMOVE,
      NUM_PENS,
   };
   enum ActionType {
      ACTION_DEFAULT,
      NUM_ACTIONS,
   };
   enum MoveType {
      MOVE_LINE,
      MOVE_ELLIPSE,
      MOVE_SPLINE,
      NUM_MOVES,
   };

   int _main;
   int _sub;
   inline EditPen(): _main(PEN_ACTION), _sub(0) { }
   inline EditPen(const EditPen& r) { operator=(*this); }
   inline EditPen& operator=(const EditPen& r) {
      _main = r._main;
      _sub = r._sub;
      return *this;
   }
};

} } }

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
