#ifndef BOOTESDANCES_MOVE_MOVECONVERTER_H
#define BOOTESDANCES_MOVE_MOVECONVERTER_H

#include <bootes/dances/IMoveModel.h>
#include "MoveModelLine.h"
#include "MoveModelEllipse.h"
#include "MoveModelSpline.h"

MoveModelLine* RealizeLine(const pb::Move::Line* idea);
pb::Move* IdealizeLine(const MoveModelLine* rea);

MoveModelEllipse* RealizeEllipse(const pb::Move::Ellipse* idea);
pb::Move* IdealizeEllipse(const MoveModelEllipse* rea);

MoveModelSpline* RealizeSpline(const pb::Move::Spline* idea);
pb::Move* IdealizeSpline(const MoveModelSpline* rea);


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
