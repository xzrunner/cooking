#ifndef _COOKING_DISPLAY_LIST_H_
#define _COOKING_DISPLAY_LIST_H_

#include <atomic>
#include <unordered_map>

#include <stddef.h>
#include <assert.h>

namespace cooking
{

struct DisplayOp;
class DisplayOpBlock;

class DisplayList
{
public:
	DisplayList();
	DisplayList(const DisplayList&) = delete;
	DisplayList(const DisplayList&&) = delete;
	DisplayList& operator = (const DisplayList&);
	DisplayList& operator = (const DisplayList&&) = delete;
	~DisplayList();

	void SetThreadIdx(int idx) { m_thread_idx = idx; }

	void Replay(int begin, int end);

	void AddOp(DisplayOp* op);
	void* AddOp(size_t op_sz);

	void DeepCopyFrom(const DisplayList& src, int pos, int count);
	
	bool Empty() const { return m_ops_sz == 0; }

	int Size() const { return m_ops_sz; }

	void Clear();

private:
	void ClearOps();

private:
	DisplayOpBlock *m_ops_head, *m_ops_tail;
	int m_ops_sz;

	int m_thread_idx;

}; // DisplayList

}

#endif // _COOKING_DISPLAY_LIST_H_