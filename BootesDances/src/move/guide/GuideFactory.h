#ifndef BOOTESDANCES_GUIDE_FACTORY_H
#define BOOTESDANCES_GUIDE_FACTORY_H

#include <bootes/dances/IGuide.h>
#include "../MoveSequence.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

// GuideRealizer に統合しよう
class GuideFactory
{
public:
   typedef std::map< std::string, const GuideFactory* > Registry;

   static const Registry& GetRegistry();
   static bool Register(GuideFactory*);
   static const GuideFactory* GetFactory(const char*);

public:
   virtual inline ~GuideFactory() { }
   virtual const char* getGuideName() const = 0;
   virtual int countSubIds() const = 0;
   virtual IGuide* createGuide(int subid) const = 0;
   virtual bool save(::google::protobuf::io::ZeroCopyOutputStream& out, const MoveSequence&) const = 0;
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
