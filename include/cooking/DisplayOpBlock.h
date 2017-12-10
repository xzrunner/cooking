#ifndef _COOKING_DISPLAY_OP_BLOCK_H_
#define _COOKING_DISPLAY_OP_BLOCK_H_

#include <cu/uncopyable.h>

namespace cooking
{

struct DisplayOp;

class DisplayOpBlock : private cu::Uncopyable
{
public:
	DisplayOpBlock();

	bool  AddOp(DisplayOp* op);
	void* AddOp(size_t op_sz);

	size_t Size() const;

	void Clear();

	static const int BLOCK_SZ = 4096;

private:
	DisplayOpBlock* m_next;

	friend class DisplayOpAllocator;
	friend class DisplayList;

}; // DisplayOpBlock

}

#endif // _COOKING_DISPLAY_OP_BLOCK_H_	