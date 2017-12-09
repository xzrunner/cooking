#ifndef _COOKING_DISPLAY_LIST_H_
#define _COOKING_DISPLAY_LIST_H_

#include <thread>
#include <atomic>
#include <unordered_map>

#include <stddef.h>
#include <assert.h>

namespace cooking
{

struct DisplayOp;

class DisplayList
{
public:
	DisplayList();
	DisplayList(const DisplayList&) = delete;
	DisplayList(const DisplayList&&) = delete;
	DisplayList& operator = (const DisplayList&);
	DisplayList& operator = (const DisplayList&&) = delete;
	~DisplayList();

	void Replay(int begin, int end);

	void AddOp(DisplayOp* op);
	void* AddOp(size_t op_sz);

	void DeepCopyFrom(const DisplayList& src, int pos, int count);
	
	bool Empty() const { return m_ops_sz == 0; }

	int Size() const { return m_ops_sz; }

	void Clear(std::thread::id thread_id);

private:
	void ClearOps(int freelist_idx = -1);

	class OpsBlock;
	class Allocator
	{
	public:
		OpsBlock* Alloc();
		void Free(OpsBlock*);

		OpsBlock* Alloc(int freelist_idx);
		void Free(int freelist_idx, OpsBlock* block);

	private:
		int QueryFreelistIdx(std::thread::id thread_id) const;

	private:
		std::vector<std::pair<std::thread::id, OpsBlock*>> m_freelists;

		friend class DisplayList;

	}; // Allocator

private:
	OpsBlock *m_ops_head, *m_ops_tail;
	int m_ops_sz;

	Allocator m_alloc;

}; // DisplayList

}

#endif // _COOKING_DISPLAY_LIST_H_