#include "cooking/DisplayList.h"
#include "cooking/RecordedOp.h"
#include "cooking/RecordedOp.h"

namespace cooking
{

DisplayList::DisplayList()
{
}

DisplayList::~DisplayList()
{
}

void DisplayList::Replay()
{
	for (int i = 0, n = m_ops.size(); i < n; ++i) {
		m_ops[i]->Replay();
	}
}

void DisplayList::Clear()
{
	m_alloc.Rewind();
	m_ops.clear();
}

}