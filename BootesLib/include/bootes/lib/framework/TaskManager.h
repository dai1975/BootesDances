#ifndef BOOTES_LIB_FRAMEWORK_TASKMANAGER_H
#define BOOTES_LIB_FRAMEWORK_TASKMANAGER_H

#include "macros.h"
#include "Task.h"
#include <list>

namespace bootes { namespace lib { namespace framework {

class TaskManager
{
public:
	TaskManager();
   virtual ~TaskManager();

	void attach(TaskPtr task);
	void update(int deltaMilliSecond);

private:
	typedef std::list< TaskPtr > t_list;
	t_list _list;
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
