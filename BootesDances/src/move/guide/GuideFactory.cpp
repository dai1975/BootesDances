#include "GuideFactory.h"

namespace {

class Static
{
public:
   GuideFactory::Registry registry;

   Static() {
   }

   ~Static() {
      for (GuideFactory::Registry::iterator i = registry.begin(); i != registry.end(); ++i) {
         GuideFactory* p = const_cast< GuideFactory* >(i->second);
         delete p;
      }
      registry.clear();
   }
};
Static _static;

}

const GuideFactory::Registry& GuideFactory::GetRegistry()
{
   return _static.registry;
}

bool GuideFactory::Register(GuideFactory* p)
{
   Registry& registry = _static.registry;
   const char* name = p->getGuideName();
   Registry::iterator i = registry.find(name);
   if (i != registry.end()) {
      return false;
   }
   registry.insert( Registry::value_type(name,p) );
   return true;
}

const GuideFactory* GuideFactory::GetFactory(const char* name)
{
   Registry& registry = _static.registry;
   Registry::iterator i = registry.find(std::string(name));
   if (i == registry.end()) {
      return NULL;
   }
   return i->second;
}

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * tab-width: 8
 * End:
 */
