#include "cooking/DisplayList.h"
#include "cooking/DisplayOp.h"
#include "cooking/DisplayOpFunc.h"

#include <algorithm>

#include <assert.h>

namespace cooking
{

class DisplayList::OpsBlock
{
public:

	OpsBlock() 
		: m_next(nullptr) 
	{
		Clear();
	}

	bool AddOp(DisplayOp* op)
	{
		uint16_t* data = reinterpret_cast<uint16_t*>(this + 1);
		uint16_t tot = data[0];
		uint16_t* index = data + 1;
		uint16_t end_start = index[tot];

		size_t op_sz = DisplayOpFunc::Size(*op);
		assert(op_sz < BLOCK_SZ - sizeof(*this) - sizeof(uint16_t) * 3);
		int space = end_start - sizeof(*this) - sizeof(uint16_t) * tot;
		if (static_cast<size_t>(space) < op_sz + sizeof(uint16_t)) {
			return false;
		}

		// add tot count
		++data[0];

		// new pos
		end_start -= static_cast<uint16_t>(op_sz);
		index[tot + 1] = end_start;

		// new data
		memcpy(reinterpret_cast<uint8_t*>(this) + end_start, op, op_sz);

		return true;
	}

	void* AddOp(size_t op_sz)
	{
		uint16_t* data = reinterpret_cast<uint16_t*>(this + 1);
		uint16_t tot = data[0];
		uint16_t* index = data + 1;
		uint16_t end_start = index[tot];

		assert(op_sz < BLOCK_SZ - sizeof(*this) - sizeof(uint16_t) * 3);
		int space = end_start - sizeof(*this) - sizeof(uint16_t) * tot;
		if (static_cast<size_t>(space) < op_sz + sizeof(uint16_t)) {
			return nullptr;
		}

		// add tot count
		++data[0];

		// new pos
		end_start -= static_cast<uint16_t>(op_sz);
		index[tot + 1] = end_start;

		return reinterpret_cast<uint8_t*>(this) + end_start;
	}

	size_t Size() const
	{
		const uint16_t* data = reinterpret_cast<const uint16_t*>(this + 1);
		return data[0];
	}

	DisplayOp* operator [] (size_t idx)
	{
		if (idx < 0 || idx >= Size()) {
			return nullptr;
		}

		const uint16_t* data = reinterpret_cast<const uint16_t*>(this + 1);
		const uint16_t* index = data + 1;
		return reinterpret_cast<DisplayOp*>(reinterpret_cast<uint8_t*>(this) + index[idx + 1]);
	}

	void Clear()
	{
		uint16_t* data = reinterpret_cast<uint16_t*>(this + 1);
		data[0] = 0;        // tot count
		data[1] = BLOCK_SZ; // index 0 start
	}

	static const int BLOCK_SZ = 4096;
	
private:
	OpsBlock* m_next;

	friend class DisplayList;

}; // DisplayOpsBlock

DisplayList::OpsBlock* DisplayList::m_freelist = nullptr;

DisplayList::DisplayList()
	: m_ops_head(nullptr)
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

	OpsBlock* ptr_b = m_ops_head;
	assert(ptr_b);
	int ptr_b_start = 0;
	size_t ptr_b_sz = ptr_b->Size();
	while (ptr_b_start + static_cast<int>(ptr_b_sz) <= begin) {
		ptr_b = ptr_b->m_next;
		ptr_b_start += ptr_b_sz;
		ptr_b_sz = ptr_b->Size();
	}
	assert(ptr_b_start + static_cast<int>(ptr_b_sz) > begin && begin >= ptr_b_start);

	int ptr_op_idx = begin - ptr_b_start;
	int left = end - begin;
	while (true)
	{
		DisplayOp* s_op = (*ptr_b)[ptr_op_idx];
		DisplayOpFunc::Replay(s_op);
		if (--left == 0) {
			break;
		} else {
			++ptr_op_idx;
			if (ptr_op_idx == ptr_b_sz) {
				ptr_b = ptr_b->m_next;
				ptr_op_idx = 0;
				ptr_b_sz = ptr_b->Size();
			}
		}
	}
}

void DisplayList::AddOp(DisplayOp* op)
{
	if (!m_ops_head) 
	{
		assert(!m_ops_tail);
		OpsBlock* b = Alloc();
		m_ops_head = m_ops_tail = b;
		m_ops_head->AddOp(op);
	} 
	else 
	{
		if (!m_ops_tail->AddOp(op)) 
		{
			OpsBlock* new_b = Alloc();
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
		OpsBlock* b = Alloc();
		m_ops_head = m_ops_tail = b;
		ret = m_ops_head->AddOp(op_sz);
	}
	else
	{
		ret = m_ops_tail->AddOp(op_sz);
		if (!ret)
		{
			OpsBlock* new_b = Alloc();
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

	OpsBlock* ptr_b = src.m_ops_head;
	assert(ptr_b);
	int ptr_b_start = 0;
	size_t ptr_b_sz = ptr_b->Size();
	while (ptr_b_start + static_cast<int>(ptr_b_sz) <= begin) {
		ptr_b = ptr_b->m_next;
		ptr_b_start += ptr_b_sz;
		ptr_b_sz = ptr_b->Size();
	}
	assert(ptr_b_start + static_cast<int>(ptr_b_sz) > begin && begin >= ptr_b_start);

	int ptr_op_idx = begin - ptr_b_start;
	int left = end - begin;
	while (true)
	{
		DisplayOp* s_op = (*ptr_b)[ptr_op_idx];
		size_t sz = DisplayOpFunc::Size(*s_op);
		void* d_op = AddOp(sz);
		memcpy(d_op, s_op, sz);
		if (--left == 0) {
			break;
		} else {
			++ptr_op_idx;
			if (ptr_op_idx == ptr_b_sz) {
				ptr_b = ptr_b->m_next;
				ptr_b_sz = ptr_b->Size();
				ptr_op_idx = 0;
			}
		}
	}
}

void DisplayList::ClearOps()
{
	OpsBlock* b = m_ops_head;
	while (b) {
		OpsBlock* next = b->m_next;
		Free(b);
		b = next;
	}
	m_ops_head = nullptr;
	m_ops_tail = nullptr;
	m_ops_sz = 0;
}

DisplayList::OpsBlock* DisplayList::Alloc()
{
	OpsBlock* ret = nullptr;
	if (m_freelist) {
		ret = m_freelist;
		m_freelist = m_freelist->m_next;
		ret->m_next = nullptr;
	} else {
		void* ptr = malloc(OpsBlock::BLOCK_SZ);
		ret = new (ptr) OpsBlock();
		ret->m_next = nullptr;
	}
	return ret;
}

void DisplayList::Free(OpsBlock* b)
{
	b->Clear();
	b->m_next = m_freelist;
	m_freelist = b;
}

}