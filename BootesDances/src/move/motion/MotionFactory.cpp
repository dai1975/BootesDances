#include "MotionFactory.h"

namespace {

class Static
{
public:
   MotionFactory::Registry registry;

   Static() {
   }

   ~Static() {
      for (MotionFactory::Registry::iterator i = registry.begin(); i != registry.end(); ++i) {
         MotionFactory* p = const_cast< MotionFactory* >(i->second);
         delete p;
      }
      registry.clear();
   }
};
Static _static;

}

const MotionFactory::Registry& MotionFactory::GetRegistry()
{
   return _static.registry;
}

bool MotionFactory::Register(MotionFactory* p)
{
   Registry& registry = _static.registry;
   const std::string name = p->getMotionName();
   Registry::iterator i = registry.find(name);
   if (i != registry.end()) {
      return false;
   }
   registry.insert( Registry::value_type(name,p) );
   return true;
}

const MotionFactory* MotionFactory::GetFactory(const char* name)
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
