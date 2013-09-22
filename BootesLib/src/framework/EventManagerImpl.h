#ifndef BOOTES_LIB_SRC_FRAMEWROK_EVENTMANAGERIMPL_H
#define BOOTES_LIB_SRC_FRAMEWROK_EVENTMANAGERIMPL_H

#include "../include.h"
#include <bootes/lib/framework/Event.h>
#include <bootes/lib/util/Timer.h>
#include <list>
#include <map>
#include <set>
#include <string>

namespace bootes { namespace lib { namespace framework {

class EventManagerImpl: public EventManager
{
public:
   EventManagerImpl();
   virtual ~EventManagerImpl();

   virtual bool subscribe(intptr_t eventId, EventListener*);
   virtual bool unsubscribe(intptr_t eventId, EventListener*);
   virtual void clock(const GameTime*, double t1);

   virtual void queue(const Event*);
   virtual void deliver(const GameTime*, const Event*);

protected:
   virtual bool registerEvent(const char* name, intptr_t* ret_evid);
   bool lock();
   void unlock();

private:
   typedef std::set< std::string > t_registry;
   t_registry _registry;

   typedef std::set< EventListener* > t_listeners;
   typedef std::map< intptr_t, t_listeners > t_subscribes;
   t_subscribes _subscribes;

   struct SubscribeLater {
      bool subscribe;
      intptr_t eventId;
      EventListener* pListener;
   };
   typedef std::list< SubscribeLater > t_subscribe_laters;
   t_subscribe_laters _subscribe_laters;

   void subscribeLater(bool subscribe, intptr_t eventId, EventListener*);
   void processSubscribeLater();
   void subscribeNow(intptr_t eventId, EventListener*);
   void unsubscribeNow(intptr_t eventId, EventListener*);

   HANDLE _mutex;

   typedef std::list< Event* > t_queue;
   t_queue _queue;
   t_queue _tmp_queue;
   ::bootes::lib::util::Timer _timer;
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
