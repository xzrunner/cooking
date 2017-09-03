#ifndef _COOKING_RENDER_TASK_H_
#define _COOKING_RENDER_TASK_H_

#include "cooking/DisplayList.h"

#include <CU_Singleton.h>
#include <multitask/Task.h>

#include <stdint.h>

namespace cooking
{

class RenderTask : public mt::Task
{
public:
	RenderTask(const void* obj, void* context);

	virtual void Run();

	void Flush();

	uint32_t GetID() const { return m_id; }
	
	DisplayList& GetDisplayList() { return m_dlist; }

	void Initialize(const void* obj, void* context);
	void Terminate();

	static void ResetNextID() { m_next_id = 0; }

	static void RegisterCallback(void (*draw_cb)(const void* obj, void* context, DisplayList* dlist), 
		void (*release_cb)(void* context));

public:
	static const unsigned int TASK_TYPE = 1;

private:
	int m_id;

	DisplayList m_dlist;

	const void* m_obj;
	void* m_context;

private:
	static uint32_t m_next_id;

}; // RenderTask

class RenderTaskMgr
{
public:
	RenderTask* Fetch(const void* obj, void* context);

	void AddResult(RenderTask* task);

	bool IsEmpty() { return m_count == 0; }

	void Flush();

private:
	int m_count;

	int m_max_id;

	mt::TaskQueue m_freelist;
	mt::SafeTaskQueue m_result;

	SINGLETON_DECLARATION(RenderTaskMgr)

}; // RenderTaskMgr

}

#endif // _COOKING_RENDER_TASK_H_