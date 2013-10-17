#include "EventManagerImpl.h"
#include <assert.h>

namespace bootes { namespace lib { namespace framework {

//----------------------------------------------------------
EventManagerImpl::EventManagerImpl()
{
   _mutex = CreateMutex(NULL, FALSE, NULL);
}

EventManagerImpl::~EventManagerImpl()
{
   CloseHandle(_mutex);
}

bool EventManagerImpl::registerEvent(const char* name, intptr_t* ret_evid)
{
   std::string s(name);
   t_registry::iterator i = _registry.find(s);
   if (i != _registry.end()) {
      return false;
   }
   i = _registry.insert(i, s);
   assert(i != _registry.end());
   
   const std::string* p = &(*i);
   *ret_evid = reinterpret_cast< intptr_t >(p);
   return true;
}

bool EventManagerImpl::lock()
{
   return (WaitForSingleObject(_mutex, INFINITE) == WAIT_OBJECT_0);
}
void EventManagerImpl::unlock()
{
   ReleaseMutex(_mutex);
}

bool EventManagerImpl::subscribe(intptr_t eventId, EventListener* pListener)
{
   subscribeLater(true, eventId, pListener);
   return true;
}
bool EventManagerImpl::unsubscribe(intptr_t eventId, EventListener* pListener)
{
   subscribeLater(false, eventId, pListener);
   return true;
}

void EventManagerImpl::subscribeLater(bool subscribe, intptr_t eventId, EventListener* pListener)
{
   SubscribeLater w;
   w.subscribe = subscribe;
   w.eventId = eventId;
   w.pListener = pListener;
   _subscribe_laters.push_back(w);
}

void EventManagerImpl::processSubscribeLater()
{
   t_subscribe_laters::iterator i;
   for (i = _subscribe_laters.begin(); i != _subscribe_laters.end(); ++i) {
      SubscribeLater& w = *i;
      if (w.subscribe) {
         subscribeNow(w.eventId, w.pListener);
      } else {
         unsubscribeNow(w.eventId, w.pListener);
      }
   }
   _subscribe_laters.clear();
}

void EventManagerImpl::subscribeNow(intptr_t eventId, EventListener* pListener)
{
   t_listeners* pl;
   {
      t_subscribes::iterator i = _subscribes.find(eventId);
      if (i == _subscribes.end()) {
         i = _subscribes.insert(i, t_subscribes::value_type(eventId, t_listeners()));
      }
      pl = &(i->second);
   }

   {
      t_listeners::iterator i = pl->find(pListener);
      if (i != pl->end()) {
         return;
      }
      pl->insert(i, pListener);
      pListener->onSubscribe(this);
   }
}

void EventManagerImpl::unsubscribeNow(intptr_t eventId, EventListener* pListener)
{
   t_listeners* pl;
   {
      t_subscribes::iterator i = _subscribes.find(eventId);
      if (i == _subscribes.end()) {
         return;
      }
      pl = &(i->second);
   }

   {
      t_listeners::iterator i = pl->find(pListener);
      if (i == pl->end()) {
         return;
      }
      pl->erase(i);
      pListener->onUnsubscribe(this);
   }
}

void EventManagerImpl::deliver(const GameTime* gt, const Event* ev)
{
   intptr_t id = ev->getEventId();
   t_subscribes::iterator i = _subscribes.find(id);
   if (i == _subscribes.end()) { return; }
   t_listeners& l = i->second;
   
   for (t_listeners::iterator j = l.begin(); j != l.end(); ++j) {
      (*j)->onEvent(gt, ev);
   }
}

void EventManagerImpl::queue(const Event* ev)
{
   lock();
   _queue.push_back(ev->clone());
   unlock();
}

void EventManagerImpl::clock(const GameTime* gt, double t1)
{
   double t;
   _timer.reset();
   _timer.start();

   processSubscribeLater();
   _timer.get(&t, NULL);
   //if (t1 <= t) { return; }

   // _tmp_queue はコンストラクタの負荷を省くためメンバにしているだけ
   // 関数を越えて状態は持たない
   _tmp_queue.clear();
   lock(); {
      _tmp_queue.swap(_queue);
   }; unlock();
   int cnt = 0;
   while (! _tmp_queue.empty()) {
      Event* ev = _tmp_queue.front();
      _tmp_queue.pop_front();
      deliver(gt, ev);
      delete ev;

      if (10 < cnt) {
         _timer.get(&t, NULL);
         if (t1 <= t) { return; }
      } else {
         ++cnt;
      }
   }
   lock(); {
      _tmp_queue.swap(_queue);
      for (t_queue::iterator i = _tmp_queue.begin(); i != _tmp_queue.end(); ++i) {
         _queue.push_back(*i);
      }
   }; unlock();
   _tmp_queue.clear();
}

} } }

/**
 * Local Variables:
 * mode: c++
 * coding: utf-8-dos
 * indent-tabs-mode: nil
 * tab-width: 8
 * c-basic-offset: 3
 * End:
 */
