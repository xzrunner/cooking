#include "cooking/RenderTask.h"

#include <multitask/ThreadPool.h>

namespace cooking
{

uint32_t RenderTask::m_next_id = 0;

static void (*DRAW_CB)(const void* obj, void* context, DisplayList* dlist);
static void (*RELEASE_CB)(void* context);

RenderTask::RenderTask(const void* obj, void* context)
	: mt::Task(TASK_TYPE)
	, m_id(m_next_id++)
	, m_obj(obj)
	, m_context(context)
{
}

void RenderTask::Run()
{
	if (DRAW_CB) {
		DRAW_CB(m_obj, m_context, &m_dlist);
	}
}

void RenderTask::Flush()
{
	m_dlist.Replay();
}

void RenderTask::Initialize(const void* obj, void* context)
{
	m_obj = obj;
	m_context = context;
}

void RenderTask::Terminate()
{
	if (RELEASE_CB) {
		RELEASE_CB(m_context);
	}
}

void RenderTask::RegisterCallback(void (*draw_cb)(const void* obj, void* context, DisplayList* dlist), 
								  void (*release_cb)(void* context))
{
	DRAW_CB = draw_cb;
	RELEASE_CB = release_cb;
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

RenderTask* RenderTaskMgr::Fetch(const void* obj, void* context)
{
	++m_count;
	mt::Task* t = m_freelist.Front();
	RenderTask* tt = static_cast<RenderTask*>(t);
	if (!t) {
		tt = new RenderTask(obj, context);
	} else {
		m_freelist.Pop();
		tt->Initialize(obj, context);
	}
	return tt;
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
			t->Terminate();
			m_freelist.Push(t);
			--m_count;
		}
	}

	delete[] tasks;

	RenderTask::ResetNextID();
}

}