#ifndef BOOTESDANCES_EV_EDITOR_H
#define BOOTESDANCES_EV_EDITOR_H

#include <bootes/lib/booteslib.h>
#include <string>

class EvEditorPen
   : public ::bootes::lib::framework::EventTmpl< EvEditorPen >
{
public:
   static const char* GetEventName() { return "EditorPen"; }

   int _main;
   int _sub;

   EvEditorPen() { }
   EvEditorPen(const EvEditorPen& r) { operator=(r); }
   EvEditorPen& operator=(const EvEditorPen& r) {
      _main = r._main;
      _sub  = r._sub;
      return *this;
   }
};

inline void Register_EvEditor(::bootes::lib::framework::EventManager* m)
{
   assert(m->registerEvent< EvEditorPen >());
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
