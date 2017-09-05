#ifndef _COOKING_RENDER_TASK_H_
#define _COOKING_RENDER_TASK_H_

#include <CU_Singleton.h>
#include <multitask/Task.h>

#include <stdint.h>

namespace cooking
{

class RenderTask : public mt::Task
{
public:
	RenderTask();
	
	virtual void Run() = 0;
	virtual void Flush() = 0;
	
	uint32_t GetID() const { return m_id; }

	static void ResetNextID() { m_next_id = 0; }

private:
	int m_id;

private:
	static uint32_t m_next_id;

}; // RenderTask

class RenderTaskMgr
{
public:
	void AddResult(RenderTask* task);

	void AddCount() { ++m_count; }

	bool IsEmpty() { return m_count == 0; }

	void Flush();

private:
	int m_count;

	int m_max_id;

	mt::SafeTaskQueue m_result;

	SINGLETON_DECLARATION(RenderTaskMgr)

}; // RenderTaskMgr

}

#endif // _COOKING_RENDER_TASK_H_