#ifndef BOOTESDANCES_EV_MOUSE_H
#define BOOTESDANCES_EV_MOUSE_H

#include <bootes/lib/booteslib.h>
#include <string>

class EvMouseDown: public ::bootes::lib::framework::EventTmpl< EvMouseDown >
{
   typedef EvMouseDown t_self;
public:
   static const char* GetEventName() { return "MouseDown"; }

   float _x, _y;

   inline EvMouseDown(): _x(0), _y(0) { }
   inline EvMouseDown(float x_, float y_): _x(x_), _y(y_) { }
   inline EvMouseDown(const t_self& r) { operator=(r); }
   inline t_self& operator=(const t_self& r) {
      _x = r._x;
      _y = r._y;
      return *this;
   }
};
class EvMouseUp: public ::bootes::lib::framework::EventTmpl< EvMouseUp >
{
   typedef EvMouseUp t_self;
public:
   static const char* GetEventName() { return "MouseUp"; }

   float _x, _y;

   inline EvMouseUp(): _x(0), _y(0) { }
   inline EvMouseUp(float x_, float y_): _x(x_), _y(y_) { }
   inline EvMouseUp(const t_self& r) { operator=(r); }
   inline t_self& operator=(const t_self& r) {
      _x = r._x;
      _y = r._y;
      return *this;
   }
};
class EvMouseMove: public ::bootes::lib::framework::EventTmpl< EvMouseMove >
{
   typedef EvMouseMove t_self;
public:
   static const char* GetEventName() { return "MouseMove"; }

   float _x, _y;

   inline EvMouseMove(): _x(0), _y(0) { }
   inline EvMouseMove(float x_, float y_): _x(x_), _y(y_) { }
   inline EvMouseMove(const t_self& r) { operator=(r); }
   inline t_self& operator=(const t_self& r) {
      _x = r._x;
      _y = r._y;
      return *this;
   }
};

inline void Register_EvMouse(::bootes::lib::framework::EventManager* m)
{
   assert(m->registerEvent< EvMouseDown >());
   assert(m->registerEvent< EvMouseUp >());
   assert(m->registerEvent< EvMouseMove >());
}

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
