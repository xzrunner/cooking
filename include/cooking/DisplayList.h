#ifndef _COOKING_DISPLAY_LIST_H_
#define _COOKING_DISPLAY_LIST_H_

#include <memmgr/LinearAllocator.h>

#include <assert.h>

namespace cooking
{

struct DisplayOp;

class DisplayList
{
public:
	DisplayList(mm::FreelistAllocator* freelist_alloc);
	DisplayList(const DisplayList&) = delete;
	DisplayList(const DisplayList&&) = delete;
	DisplayList& operator = (const DisplayList&);
	DisplayList& operator = (const DisplayList&&) = delete;
	~DisplayList();

	void Replay(int begin, int end);

	void AddOp(DisplayOp* op);

	void DeepCopyFrom(const DisplayList& src, int pos, int count);
	
	mm::LinearAllocator& GetAlloc() { return m_alloc; }

	bool Empty() const { return m_ops_sz == 0; }

	int Size() const { return m_ops_sz; }

private:
	static const int BLOCK_CAP = 128;

	struct OpArrayBlock
	{
		OpArrayBlock()
			: ops(nullptr)
			, ops_sz(0)
			, next(nullptr)
		{}

		bool IsFull() const { return ops_sz == BLOCK_CAP; }

		void Add(DisplayOp* op) { ops[ops_sz++] = op; }

		DisplayOp** ops;
		int ops_sz;

		OpArrayBlock* next;
	};

	struct OpArrayMemPool
	{
		OpArrayMemPool()
			: freelist(nullptr) 
		{}

		OpArrayBlock* Alloc() 
		{
			OpArrayBlock* ret = nullptr;
			if (freelist) {
				ret = freelist;
				freelist = freelist->next;
				ret->ops_sz = 0;
				ret->next = nullptr;
			} else {
				ret = (OpArrayBlock*)malloc(sizeof(OpArrayBlock) + sizeof(DisplayOp*) * BLOCK_CAP);
				ret->ops = reinterpret_cast<DisplayOp**>(ret + 1);
				ret->ops_sz = 0;
				ret->next = nullptr;
			}
			return ret;
		}
		void Free(OpArrayBlock* b)
		{
			b->ops_sz = 0;
			b->next = freelist;
			freelist = b;
		}

		OpArrayBlock* freelist;
	};

private:
	void ClearOps();

private:
	mm::LinearAllocator m_alloc;

	OpArrayBlock *m_ops_head, *m_ops_tail;
	int m_ops_sz;

	static OpArrayMemPool m_pool;

}; // DisplayList

}

#endif // _COOKING_DISPLAY_LIST_H_