#include "cooking/DisplayOpAllocator.h"
#include "cooking/DisplayOpBlock.h"

#include <assert.h>

namespace cooking
{

CU_SINGLETON_DEFINITION(DisplayOpAllocator)

DisplayOpAllocator::DisplayOpAllocator()
{
}

void DisplayOpAllocator::Init(int thread_num)
{
	if (m_freelists.empty()) {
		m_freelists.resize(thread_num, nullptr);
	}
}

DisplayOpBlock* DisplayOpAllocator::Alloc(int thread_idx)
{
	DisplayOpBlock* ret = nullptr;
	assert(thread_idx >= 0 && thread_idx < static_cast<int>(m_freelists.size()));
	auto& freelist = m_freelists[thread_idx];
	if (freelist) {
		ret = freelist;
		freelist = ret->m_next;
		ret->m_next = nullptr;
	} else {
		void* ptr = malloc(DisplayOpBlock::BLOCK_SZ);
		ret = new (ptr) DisplayOpBlock();
		ret->m_next = nullptr;
	}
	return ret;
}

void DisplayOpAllocator::Free(int thread_idx, DisplayOpBlock* block)
{
	block->Clear();

	assert(thread_idx >= 0 && thread_idx < static_cast<int>(m_freelists.size()));
	auto& freelist = m_freelists[thread_idx];
	assert(block != freelist);
	block->m_next = freelist;
	freelist = block;
}

}