#ifndef BOOTES_LIB_FRAMEWORK_EVENT_H
#define BOOTES_LIB_FRAMEWORK_EVENT_H

#include "macros.h"
#include <boost/cstdint.hpp>

namespace bootes { namespace lib { namespace framework {

/* usage:
class FooEvent: public EventTmpl< FooEvent >
{
public:
  static const char* sGetEventName() { return "Foo"; }

  ...
};
...
EventManager& em = ...;
em.RegisterEvent< FooEvent >();
...
FooEvent* pev = new FooEvent();
em.push(pev);
*/

class Event
{
protected:
   inline explicit Event(intptr_t id, const char* name): _name(name), _id(id) { }
public:
   inline virtual ~Event() { }
   virtual Event* clone() const = 0;

   inline intptr_t getEventId() const { return _id; }
   inline const char* getEventName() const { return _name; }

private:
   const char* _name;
   intptr_t _id;
};

template <typename T>
class EventTmpl: public Event
{
public:
   static inline const char* GetEventName() { return T::GetEventName(); }
   static inline intptr_t GetEventId() { return s_eventId; }
   inline intptr_t getEventId() const { return s_eventId; }

   inline EventTmpl(): Event(s_eventId, T::GetEventName()) { }
   inline Event* clone() const { return new T(*static_cast< const T* >(this)); }

private:
   friend class EventManager;
   static intptr_t s_eventId; // = reinterpret_cast< intptr_t >(&s_eventId);
};
template <typename T> intptr_t EventTmpl< T >::s_eventId = reinterpret_cast< intptr_t >(&EventTmpl< T >::s_eventId);


class EventListener
{
public:
   virtual void onEvent(const Event* ev) = 0;
   virtual void onSubscribe(EventManager*) { }
   virtual void onUnsubscribe(EventManager*) { }
   
protected:
   template <typename SELF, typename EV>
   bool tryDispatch(const Event* ev, void(SELF::*pf)(const EV*)) {
      intptr_t id = ev->getEventId();
      if (EV::GetEventId() == id) {
         SELF* self = static_cast< SELF* >(this);
         (self->*pf)( static_cast< const EV* >(ev) );
         return true;
      }
      return false;
   }
};

class EventManager
{
public:
   inline EventManager() { }
   inline virtual ~EventManager() { }

   template <typename T> inline bool registerEvent() {
      return registerEvent(T::GetEventName(), &EventTmpl< T >::s_eventId);
   }
   template <typename T> inline bool subscribe(EventListener* p) {
      return subscribe(T::GetEventId(), p);
   }
   template <typename T> inline bool unsubscribe(EventListener* p) {
      return unsubscribe(T::GetEventId(), p);
   }

   virtual void clock(int limitMsec) = 0;
   virtual void queue(const Event* ev) = 0;
   virtual void deliver(const Event* ev) = 0;
   
protected:
   virtual bool registerEvent(const char* name, intptr_t* ret_evid) = 0;
   virtual bool subscribe(intptr_t eventId, EventListener*) = 0;
   virtual bool unsubscribe(intptr_t eventId, EventListener*) = 0;
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
