#include "cooking/DisplayList.h"
#include "cooking/DisplayOp.h"
#include "cooking/ReplayOpBuilder.h"

namespace cooking
{

DisplayList::DisplayList(mm::FreelistAllocator* freelist_alloc,
	                     int capacity)
	: m_alloc(freelist_alloc)
{
	if (capacity > 0) {
		m_ops.reserve(capacity);
	}
}

DisplayList::~DisplayList()
{
}

DisplayList& DisplayList::operator = (const DisplayList& dlist)
{
	if (this == &dlist) {
		return *this;
	}

	m_alloc.~LinearAllocator();
	m_alloc = dlist.m_alloc;

	m_ops = dlist.m_ops;

	return *this;
}

DisplayList& DisplayList::operator = (const DisplayList&& dlist)
{
	if (this == &dlist) {
		return *this;
	}

	m_alloc.~LinearAllocator();
	m_alloc = std::move(dlist.m_alloc);

	m_ops = std::move(dlist.m_ops);

	return *this;
}

void DisplayList::Replay(int begin, int end)
{
	ReplayOpBuilder::Replay(m_ops, begin, end);
}

}