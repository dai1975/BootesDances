#include "../include.h"
#include <bootes/lib/framework/Task.h>

namespace bootes { namespace lib { namespace framework {

//-----------------------------------------------------------
Task::Task()
{
   _next = TaskPtr();
   _state = S_INIT;
   _bKill  = false;
   _bPause = false;
   _pManager = NULL;
}

Task::~Task()
{
   if (_next) {
      _next = TaskPtr();
   }
}

Task* Task::setNext(TaskPtr next)
{
   _next = next;
   return this;
}

void Task::attach(TaskManager* p)
{
   assert(_pManager == NULL);
   _pManager = p;
   onAttach();
}
void Task::detach()
{
   assert(_pManager != NULL);
   _pManager = NULL;
   onDetach();
}

void Task::kill()
{
   _bKill = true;
}
void Task::pause(bool b)
{
   _bPause = b;
}

void Task::update(int deltaMilliSecond)
{
   if (! isUpdatable()) { return; }

   switch (_state)
   {
   case S_INIT:
      if (_bKill) {
         _state = S_FIN;
      } else {
         onInitialize();
         _state = S_RUN;
      }
      break;

   case S_RUN:
      if (_bKill) {
         onFinalize();
         _state = S_FIN;
      } else if (! _bPause) {
         onUpdate(deltaMilliSecond);
      }
      break;
      
   case S_FIN:
      break;
   }
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
