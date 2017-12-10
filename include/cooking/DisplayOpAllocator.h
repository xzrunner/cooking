#ifndef _COOKING_DISPLAY_OP_ALLOCATOR_H_
#define _COOKING_DISPLAY_OP_ALLOCATOR_H_

#include <cu/cu_macro.h>

#include <vector>

namespace cooking
{

class DisplayOpBlock;

class DisplayOpAllocator
{
public:
	void Init(int thread_num);

	DisplayOpBlock* Alloc(int thread_idx);
	void Free(int thread_idx, DisplayOpBlock* block);

private:
	// heap for each thread
	std::vector<DisplayOpBlock*> m_freelists;

	CU_SINGLETON_DECLARATION(DisplayOpAllocator)

}; // DisplayOpAllocator

}

#endif // _COOKING_DISPLAY_OP_ALLOCATOR_H_