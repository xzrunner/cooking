#include "cooking/DisplayOpBlock.h"
#include "cooking/DisplayOpFunc.h"

#include <stdint.h>
#include <assert.h>

namespace cooking
{

DisplayOpBlock::DisplayOpBlock()
	: m_next(nullptr)
{
	Clear();
}

bool DisplayOpBlock::AddOp(DisplayOp* op)
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

void* DisplayOpBlock::AddOp(size_t op_sz)
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

size_t DisplayOpBlock::Size() const
{
	const uint16_t* data = reinterpret_cast<const uint16_t*>(this + 1);
	return data[0];
}

void DisplayOpBlock::Clear()
{
	uint16_t* data = reinterpret_cast<uint16_t*>(this + 1);
	data[0] = 0;        // tot count
	data[1] = BLOCK_SZ; // index 0 start
}

}