#include "cooking/DisplayList.h"
#include "cooking/DisplayOp.h"
#include "cooking/DisplayOpFunc.h"

#include <algorithm>

#include <assert.h>

namespace cooking
{

DisplayList::OpArrayMemPool DisplayList::m_pool;

DisplayList::DisplayList(mm::FreelistAllocator* freelist_alloc)
	: m_alloc(freelist_alloc)
	, m_ops_head(nullptr)
	, m_ops_tail(nullptr)
	, m_ops_sz(0)
{
}

DisplayList::~DisplayList()
{
	ClearOps();
}

DisplayList& DisplayList::operator = (const DisplayList& dlist)
{
	if (this == &dlist) {
		return *this;
	}

	m_alloc = dlist.m_alloc;

	ClearOps();
	m_ops_head = dlist.m_ops_head;
	m_ops_tail = dlist.m_ops_tail;
	m_ops_sz = dlist.m_ops_sz;
	const_cast<DisplayList&>(dlist).m_ops_head = nullptr;
	const_cast<DisplayList&>(dlist).m_ops_tail = nullptr;
	const_cast<DisplayList&>(dlist).m_ops_sz = 0;

	return *this;
}

void DisplayList::Replay(int begin, int end)
{
	if (begin == end || m_ops_sz == 0) {
		return;
	}
	if (begin < 0) {
		begin = 0;
	}
	if (end < 0) {
		end = m_ops_sz;
	}
	begin = std::min(begin, m_ops_sz);
	end = std::min(end, m_ops_sz);

	if (begin >= end || begin >= m_ops_sz) {
		return;
	}

	OpArrayBlock* ptr_b = m_ops_head;
	assert(ptr_b);
	int ptr_b_start = 0;
	while (ptr_b_start + BLOCK_CAP <= begin) {
		ptr_b = ptr_b->next;
		ptr_b_start += BLOCK_CAP;
	}
	assert(ptr_b_start + BLOCK_CAP > begin && begin >= ptr_b_start);

	int ptr_op_idx = begin - ptr_b_start;
	int left = end - begin;
	while (left-- > 0)
	{
		DisplayOp* s_op = ptr_b->ops[ptr_op_idx];
		DisplayOpFunc::Replay(s_op);
		++ptr_op_idx;
		if (ptr_op_idx == BLOCK_CAP) {
			ptr_b = ptr_b->next;
			ptr_op_idx = 0;
		}
	}
}

void DisplayList::AddOp(DisplayOp* op)
{
	if (!m_ops_head) 
	{
		assert(!m_ops_tail);
		m_ops_head = m_ops_tail = m_pool.Alloc();
		m_ops_head->Add(op);
	} 
	else 
	{
		if (m_ops_tail->IsFull()) {
			OpArrayBlock* new_b = m_pool.Alloc();
			m_ops_tail->next = new_b;
			m_ops_tail = new_b;
		}
		m_ops_tail->Add(op);
	}
	++m_ops_sz;
}

void DisplayList::DeepCopyFrom(const DisplayList& src, int pos, int count)
{
	if (count == 0) {
		return;
	}
	int sz = src.Size();
	int begin = pos < 0 ? 0 : pos;
	int end = count < 0 ? sz : begin + count;
	if (begin == end) {
		return;
	}
	assert(begin >= 0 && begin < sz && end >= 0 && end <= sz && begin < end);

	OpArrayBlock* ptr_b = src.m_ops_head;
	assert(ptr_b);
	int ptr_b_start = 0;
	while (ptr_b_start + BLOCK_CAP <= begin) {
		ptr_b = ptr_b->next;
		ptr_b_start += BLOCK_CAP;
	}
	assert(ptr_b_start + BLOCK_CAP > begin && begin >= ptr_b_start);

	int ptr_op_idx = begin - ptr_b_start;
	int left = end - begin;
	while (left-- > 0)
	{
		DisplayOp* s_op = ptr_b->ops[ptr_op_idx];
		size_t sz = DisplayOpFunc::Size(*s_op);
		DisplayOp* d_op = reinterpret_cast<DisplayOp*>(m_alloc.alloc<char>(sz));
		memcpy(d_op, s_op, sz);
		AddOp(d_op);

		++ptr_op_idx;
		if (ptr_op_idx == BLOCK_CAP) {
			ptr_b = ptr_b->next;
			ptr_op_idx = 0;
		}
	}
}

void DisplayList::ClearOps()
{
	OpArrayBlock* b = m_ops_head;
	while (b) {
		OpArrayBlock* next = b->next;
		m_pool.Free(b);
		b = next;
	}
	m_ops_head = nullptr;
	m_ops_tail = nullptr;
	m_ops_sz = 0;
}

}