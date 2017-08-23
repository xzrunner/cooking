#ifndef _COOKING_RENDER_TASK_H_
#define _COOKING_RENDER_TASK_H_

#include "cooking/DisplayList.h"

#include <multitask/Task.h>

#include <stdint.h>

namespace cooking
{

class RenderTask : public mt::Task
{
public:
	RenderTask(void (*draw)(void* obj, void* context, DisplayList* dlist), 
		void* obj, void* context);

	virtual void Run();

	uint32_t GetID() const { return m_id; }
	
	DisplayList& GetDisplayList() { return m_dlist; }

public:
	static const unsigned int TASK_TYPE = 0;

private:
	RenderTask(const RenderTask& other);

private:
	int m_id;

	DisplayList m_dlist;

	// cb
	void (*m_draw_cb)(void* obj, void* context, DisplayList* dlist);
	void *m_obj, *m_context;

private:
	static uint32_t m_next_id;

}; // RenderTask

class ThreadPool
{
public:
	static void AddTask(RenderTask* task);

	static void Flush();

}; // ThreadPool

}

#endif // _COOKING_RENDER_TASK_H_