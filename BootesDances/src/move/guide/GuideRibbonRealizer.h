#ifndef BOOTESDANCES_GUIDE_RIBBONREALIZER_H
#define BOOTESDANCES_GUIDE_RIBBONREALIZER_H

#include "GuideRealizer.h"

class GuideRibbonRealizer: public GuideRealizer
{
public:
   virtual const char* getGuideName() const;
   virtual const TCHAR* getGuideNameT() const;
   virtual int countSubIds() const;
   virtual IGuide* createGuide(int subid) const;

protected:
   virtual bool save(int fd, const MoveSequence&) const;
   virtual bool load(std::list< GuideData >* pOut, int fd) const;
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
