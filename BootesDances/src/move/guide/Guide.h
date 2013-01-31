#ifndef BOOTESDANCES_GUIDE_GUIDE_H
#define BOOTESDANCES_GUIDE_GUIDE_H

#include "../../include.h"
#include <bootes/dances/IGuide.h>
#include <bootes/dances/IMove.h>
#include <bootes/dances/proto/Stage.pb.h>

class Guide: public IGuide
{
protected:
   Guide(const Guide&);
   explicit Guide(int subid);

public:
   virtual ~Guide();
   virtual void setMove(IMove*);
   virtual int getSubId() const;

protected:
   IMove* _pMove;
   int _subid;
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
