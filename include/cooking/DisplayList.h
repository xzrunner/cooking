#ifndef _COOKING_DISPLAY_LIST_H_
#define _COOKING_DISPLAY_LIST_H_

#include <memmgr/LinearAllocator.h>

namespace cooking
{

struct DisplayOp;

class DisplayList
{
public:
	DisplayList(mm::FreelistAllocator* freelist_alloc = nullptr, int capacity = -1);
	DisplayList(const DisplayList&) = delete;
	DisplayList& operator = (const DisplayList&) = delete;
	~DisplayList();

	void Replay();

	void AddOp(DisplayOp* op) { m_ops.push_back(op); }
	
	mm::LinearAllocator& GetAlloc() { return m_alloc; }

private:
	mm::LinearAllocator m_alloc;
	mm::LinearStdAllocator<void*> m_std_alloc;

	mm::LsaVector<DisplayOp*> m_ops;

}; // DisplayList

}

#endif // _COOKING_DISPLAY_LIST_H_