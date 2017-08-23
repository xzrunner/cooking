#ifndef _COOKING_DISPLAY_LIST_H_
#define _COOKING_DISPLAY_LIST_H_

#include "cooking/LinearAllocator.h"

#include <vector>

namespace cooking
{

struct RecordedOp;

class DisplayList
{
public:
	DisplayList();
	~DisplayList();

	void Replay();

	LinearAllocator& GetAlloc() { return m_alloc; }

	void AddOp(RecordedOp* op) { m_ops.push_back(op); }

private:
	DisplayList(const DisplayList& other);
	
private:
	LinearAllocator m_alloc;

	std::vector<RecordedOp*> m_ops;

}; // DisplayList

}

#endif // _COOKING_DISPLAY_LIST_H_