#include "../include.h"
#include <bootes/lib/framework/TaskManager.h>

namespace bootes { namespace lib { namespace framework {

//-------------------------------------------------------------------
TaskManager::TaskManager()
{
}

TaskManager::~TaskManager()
{
}

void TaskManager::attach(TaskPtr t)
{
   _list.push_back(t);
   t->attach(this);
}

void TaskManager::update(int deltaMilliSecond)
{
   t_list::iterator tmp;
   t_list::iterator i = _list.begin();
   t_list::iterator end = _list.end();

   while (i != end) {
      tmp = i++;
      TaskPtr t(*tmp);

      if (! t->isUpdatable()) { continue; }

      t->update(deltaMilliSecond);

      if (t->isFinish()) {
         TaskPtr n = t->getNext();
         if (n) {
            attach(n);
         }
         t->detach();
         _list.erase(tmp);
      }
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
