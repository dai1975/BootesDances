#ifndef BOOTESDANCES_GUIDE_GUIDEREALIZER_H
#define BOOTESDANCES_GUIDE_GUIDEREALIZER_H

#include "../../include.h"
#include <bootes/dances/IGuide.h>
#include "Guide.h"
#include "../MoveSequence.h"

class GuideRealizer
{
public:
   typedef std::map< std::string, const GuideRealizer* > Registry;

   static const Registry& GetRegistry();
   static bool Register(GuideRealizer*);
   static const GuideRealizer* GetRealizer(const char*);

public:
   GuideRealizer();
   virtual ~GuideRealizer();
   virtual const char* getGuideName() const = 0;
   virtual const TCHAR* getGuideNameT() const = 0;
   virtual int countSubIds() const = 0;
   virtual IGuide* createGuide(int subid) const = 0;

public:
   static std::basic_string< TCHAR > GetFilePath(const TCHAR* basepath, const TCHAR* subdir, const TCHAR* guide);
   static bool IsExist(const TCHAR* basepath, const TCHAR* subdir, const TCHAR* guide);

   struct GuideData {
      IGuide* pGuide;
      std::string uuid;
      __int64 t0,t1;
      bool chainnext;
   };
   bool save(const TCHAR* basepath, const TCHAR* subdir, const MoveSequence&) const;
   bool load(std::list< GuideData >* pOut, const TCHAR* basepath, const TCHAR* subdir) const;

protected:
   virtual bool save(int fd, const MoveSequence&) const = 0;
   virtual bool load(std::list< GuideData >* pOut, int fd) const = 0;
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
