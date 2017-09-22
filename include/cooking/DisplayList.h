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
	DisplayList(const DisplayList&&) = delete;
	DisplayList& operator = (const DisplayList&);
	DisplayList& operator = (const DisplayList&&);
	~DisplayList();

	void Replay(int begin, int end);

	void AddOp(DisplayOp* op) { m_ops.push_back(op); }
	
	mm::LinearAllocator& GetAlloc() { return m_alloc; }

	bool Empty() const { return m_ops.empty(); }

	size_t Size() const { return m_ops.size(); }

private:
	mm::LinearAllocator m_alloc;

	std::vector<DisplayOp*> m_ops;

}; // DisplayList

}

#endif // _COOKING_DISPLAY_LIST_H_