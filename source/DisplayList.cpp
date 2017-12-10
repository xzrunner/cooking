#include "cooking/DisplayList.h"
#include "cooking/DisplayOp.h"
#include "cooking/DisplayOpFunc.h"
#include "cooking/DisplayOpBlock.h"
#include "cooking/DisplayOpAllocator.h"

#include <algorithm>

#include <assert.h>

namespace cooking
{

DisplayList::DisplayList()
	: m_ops_head(nullptr)
	, m_ops_tail(nullptr)
	, m_ops_sz(0)
	, m_thread_idx(-1)
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
	if (m_ops_sz == 0) {
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

	DisplayOpBlock* ptr_b = m_ops_head;
	assert(ptr_b);
	int ptr_b_start = 0;
	size_t ptr_b_sz = ptr_b->Size();
	assert(ptr_b_sz > 0);
	while (ptr_b_start + static_cast<int>(ptr_b_sz) <= begin) {
		ptr_b = ptr_b->m_next;
		assert(ptr_b);
		ptr_b_start += ptr_b_sz;
		ptr_b_sz = ptr_b->Size();
		assert(ptr_b_sz > 0);
	}
	assert(ptr_b_start + static_cast<int>(ptr_b_sz) > begin && begin >= ptr_b_start);

	const uint16_t* ptr_b_idx = reinterpret_cast<const uint16_t*>(ptr_b + 1) + 1;
	int ptr_op_idx = begin - ptr_b_start;
	int left = end - begin;
	while (true)
	{
		DisplayOp* s_op = reinterpret_cast<DisplayOp*>(reinterpret_cast<uint8_t*>(ptr_b) + ptr_b_idx[ptr_op_idx + 1]);
		DisplayOpFunc::Replay(s_op);
		if (--left == 0) {
			break;
		} else {
			++ptr_op_idx;
			if (ptr_op_idx == ptr_b_sz) {
				ptr_b = ptr_b->m_next;
				assert(ptr_b);
				ptr_op_idx = 0;
				ptr_b_sz = ptr_b->Size();
				assert(ptr_b_sz > 0);
				ptr_b_idx = reinterpret_cast<const uint16_t*>(ptr_b + 1) + 1;
			}
		}
	}
}

void DisplayList::AddOp(DisplayOp* op)
{
	if (!m_ops_head) 
	{
		assert(!m_ops_tail);
		DisplayOpBlock* b = DisplayOpAllocator::Instance()->Alloc(m_thread_idx);
		m_ops_head = m_ops_tail = b;
		m_ops_head->AddOp(op);
	} 
	else 
	{
		if (!m_ops_tail->AddOp(op)) 
		{
			DisplayOpBlock* new_b = DisplayOpAllocator::Instance()->Alloc(m_thread_idx);
			assert(m_ops_tail != new_b);
			m_ops_tail->m_next = new_b;
			m_ops_tail = new_b;
			m_ops_tail->AddOp(op);
		}
	}
	++m_ops_sz;
}

void* DisplayList::AddOp(size_t op_sz)
{
	void* ret = nullptr;
	if (!m_ops_head)
	{
		assert(!m_ops_tail);
		DisplayOpBlock* b = DisplayOpAllocator::Instance()->Alloc(m_thread_idx);
		m_ops_head = m_ops_tail = b;
		ret = m_ops_head->AddOp(op_sz);
	}
	else
	{
		ret = m_ops_tail->AddOp(op_sz);
		if (!ret)
		{
			DisplayOpBlock* new_b = DisplayOpAllocator::Instance()->Alloc(m_thread_idx);
			assert(m_ops_tail != new_b);
			m_ops_tail->m_next = new_b;
			m_ops_tail = new_b;
			ret = m_ops_tail->AddOp(op_sz);
		}
	}
	++m_ops_sz;

	assert(ret);
	return ret;
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

	DisplayOpBlock* src_b = src.m_ops_head;
	assert(src_b);
	int src_b_start = 0;
	size_t src_b_sz = src_b->Size();
	while (src_b_start + static_cast<int>(src_b_sz) <= begin) {
		src_b = src_b->m_next;
		src_b_start += src_b_sz;
		src_b_sz = src_b->Size();
	}
	assert(src_b_start + static_cast<int>(src_b_sz) > begin && begin >= src_b_start);

	if (!m_ops_head) {
		assert(!m_ops_tail);
		DisplayOpBlock* b = DisplayOpAllocator::Instance()->Alloc(m_thread_idx);
		m_ops_head = m_ops_tail = b;
	}
	assert(m_ops_tail);
	uint16_t* dst_data = reinterpret_cast<uint16_t*>(m_ops_tail + 1);
	int dst_tot = dst_data[0];
	uint16_t* dst_b_idx = dst_data + 1;
	int dst_end_start = dst_b_idx[dst_tot];
	int dst_space = dst_end_start - sizeof(*m_ops_tail) - sizeof(uint16_t) * dst_tot;

	const uint16_t* src_b_idx = reinterpret_cast<const uint16_t*>(src_b + 1) + 1;
	int src_op_idx = begin - src_b_start;
	int left = end - begin;
	while (true)
	{
		DisplayOp* s_op = reinterpret_cast<DisplayOp*>(reinterpret_cast<uint8_t*>(src_b) + src_b_idx[src_op_idx + 1]);
		size_t sz = src_b_idx[src_op_idx] - src_b_idx[src_op_idx + 1];

		if (static_cast<size_t>(dst_space) < sz + sizeof(uint16_t))
		{
			DisplayOpBlock* new_b = DisplayOpAllocator::Instance()->Alloc(m_thread_idx);

			assert(m_ops_tail != new_b);
			m_ops_tail->m_next = new_b;
			m_ops_tail = new_b;	

			dst_data = reinterpret_cast<uint16_t*>(m_ops_tail + 1);
			dst_tot = dst_data[0];
			assert(dst_tot == 0);
			dst_b_idx = dst_data + 1;
			dst_end_start = dst_b_idx[dst_tot];
			dst_space = dst_end_start - sizeof(*m_ops_tail) - sizeof(uint16_t) * dst_tot;
		}

		assert(static_cast<size_t>(dst_space) >= sz + sizeof(uint16_t));
		
		++dst_data[0];
		dst_tot = dst_data[0];
		dst_end_start -= static_cast<uint16_t>(sz);
		dst_b_idx[dst_tot] = dst_end_start;
		dst_space -= (sz + sizeof(uint16_t));

		memcpy(reinterpret_cast<uint8_t*>(m_ops_tail) + dst_end_start, s_op, sz);
		++m_ops_sz;

		if (--left == 0) {
			break;
		} else {
			++src_op_idx;
			if (src_op_idx == src_b_sz) {
				src_b = src_b->m_next;
				src_b_sz = src_b->Size();
				src_op_idx = 0;
				src_b_idx = reinterpret_cast<const uint16_t*>(src_b + 1) + 1;
			}
		}
	}
}

void DisplayList::Clear()
{
	ClearOps();
}

void DisplayList::ClearOps()
{
	DisplayOpBlock* b = m_ops_head;
	while (b) {
		DisplayOpBlock* next = b->m_next;
		DisplayOpAllocator::Instance()->Free(m_thread_idx, b);
		b = next;
	}
	m_ops_head = nullptr;
	m_ops_tail = nullptr;
	m_ops_sz = 0;
}

}