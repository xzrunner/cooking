#include "cooking/RenderTask.h"

namespace cooking
{

/************************************************************************/
/* class RenderTask                                                     */
/************************************************************************/

uint32_t RenderTask::m_next_id = 0;

RenderTask::RenderTask()
	: mt::Task(0)
	, m_id(m_next_id++)
{
}

/************************************************************************/
/* class RenderTaskMgr                                                  */
/************************************************************************/

SINGLETON_DEFINITION(RenderTaskMgr)

RenderTaskMgr::RenderTaskMgr()
	: m_count(0)
	, m_max_id(0)
{
}

void RenderTaskMgr::AddResult(RenderTask* task)
{
	m_result.Push(task);

	if (task->GetID() > m_max_id) {
		m_max_id = task->GetID();
	}
}

void RenderTaskMgr::Flush()
{
	RenderTask** tasks = new RenderTask*[m_max_id + 1];
	memset(tasks, 0, sizeof(tasks));
	while (mt::Task* t = m_result.TryPop())
	{
		RenderTask* tt = static_cast<RenderTask*>(t);
		tasks[tt->GetID()] = tt;
	}

	for (int i = 0; i < m_max_id + 1; ++i) {
		RenderTask* t = tasks[i];
		if (t) {
			t->Flush();
			--m_count;
		}
	}

	delete[] tasks;

	RenderTask::ResetNextID();
}

}