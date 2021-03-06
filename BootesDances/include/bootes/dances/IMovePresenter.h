#ifndef BOOTESDANCES_IMOVEPRESENTER_H
#define BOOTESDANCES_IMOVEPRESENTER_H

#include "MoveEditee.h"
#include <vector>

class IMovePresenter
{
public:
   virtual size_t getRenderedModels(std::vector<const IMove*>*) const = 0;
   virtual bool   presentLocated(float rx, float ry, MoveEditee* o) const = 0;
   virtual bool   presentNearest(float rx, float ry, MoveEditee* o) const = 0;
   virtual bool   presentNearestEdge(float rx, float ry, MoveEditee* o) const = 0;
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
