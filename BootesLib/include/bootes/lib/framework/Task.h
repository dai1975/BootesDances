#ifndef BOOTES_LIB_FRAMEWORK_TASK_H
#define BOOTES_LIB_FRAMEWORK_TASK_H

#include "macros.h"
#include <boost/shared_ptr.hpp>

namespace bootes { namespace lib { namespace framework {

class TaskManager;
class Task;

typedef boost::shared_ptr< Task > TaskPtr;

class Task
{
public:
	Task();
	virtual ~Task();
	Task* setNext(TaskPtr next);
	inline TaskPtr getNext() { return _next; }

	inline bool isAttach() const;
	inline bool isFinish() const;

	virtual void kill();
	virtual void pause(bool b);

protected:
	inline TaskManager* getTaskManager() { return _pManager; }
	inline virtual void onAttach() { };
	inline virtual void onDetach() { };
	inline virtual void onUpdate(int deltaMilliSec) { };
	inline virtual void onInitialize() { };
	inline virtual void onFinalize() { };

private:
	friend class TaskManager;
	enum STATE {
		S_INIT, S_RUN, S_FIN,
	};
	inline bool isUpdatable() const { return (_state != S_RUN || !_bPause); }
	void update(int deltaMilliSec);
	void attach(TaskManager*);
	void detach();

private:
	TaskManager* _pManager;
	TaskPtr _next;
	STATE _state;
	bool _bKill, _bPause;
};

inline bool Task::isAttach() const
{
	return (_pManager != NULL);
}
inline bool Task::isFinish() const
{
	return (_state == S_FIN);
}

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
