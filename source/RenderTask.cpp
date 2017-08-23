#include "cooking/RenderTask.h"

#include <multitask/ThreadPool.h>

namespace cooking
{

uint32_t RenderTask::m_next_id = 0;

RenderTask::RenderTask(void (*draw)(void* obj, void* context, DisplayList* dlist), 
					   void* obj, void* context)
	: mt::Task(TASK_TYPE)
	, m_id(m_next_id++)
	, m_draw_cb(draw)
	, m_obj(obj)
	, m_context(context)
{
}

void RenderTask::Run()
{
	if (m_draw_cb) {
		m_draw_cb(m_obj, m_context, &m_dlist);
	}
}

/************************************************************************/
/* class ThreadPool                                                     */
/************************************************************************/

void ThreadPool::AddTask(RenderTask* task)
{
	mt::ThreadPool::Instance()->AddTask(task);
}

void ThreadPool::Flush()
{
	mt::ThreadPool::Instance()->Lock();

	const int sz = mt::ThreadPool::THREAD_NUM;
	mt::Task* tasks[sz];
	mt::ThreadPool::Instance()->GetResult(RenderTask::TASK_TYPE, tasks);
	
	while (true)
	{
		RenderTask* task = NULL;
		int task_idx = -1;

		// choose
		for (int i = 0; i < sz; ++i) 
		{
			mt::Task* t = tasks[i];
			if (!t) {
				continue;
			}
			RenderTask* rt = static_cast<RenderTask*>(t);
			if (!task || rt->GetID() < task->GetID()) {
				task = rt;
				task_idx = i;
			}
		}

		if (task) {
			task->GetDisplayList().Replay();
			tasks[task_idx] = tasks[task_idx]->GetNext();
		} else {
			break;
		}
	}

	mt::ThreadPool::Instance()->Unlock();
}

}