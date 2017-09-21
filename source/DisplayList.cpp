#include "cooking/DisplayList.h"
#include "cooking/DisplayOp.h"
#include "cooking/ReplayOpBuilder.h"

namespace cooking
{

DisplayList::DisplayList(mm::FreelistAllocator* freelist_alloc,
	                     int capacity)
	: m_alloc(freelist_alloc)
	, m_std_alloc(m_alloc)
	, m_ops(m_std_alloc)
{
	if (capacity > 0) {
		m_ops.reserve(capacity);
	}
}

DisplayList::~DisplayList()
{
}

void DisplayList::Replay()
{
	ReplayOpBuilder::Replay(m_ops);
}

}