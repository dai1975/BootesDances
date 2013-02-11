#ifndef BOOTESDANCES_GUIDE_RIBBONREALIZER_H
#define BOOTESDANCES_GUIDE_RIBBONREALIZER_H

#include "GuideFactory.h"

class GuideRibbonRealizer: public GuideFactory
{
public:
   virtual const char* getGuideName() const;
   virtual int countSubIds() const;
   virtual IGuide* createGuide(int subid) const;
   virtual bool save(::google::protobuf::io::ZeroCopyOutputStream& out, const MoveSequence&) const;
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
