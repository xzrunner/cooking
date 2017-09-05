#ifndef _COOKING_RECORDED_OP_H_
#define _COOKING_RECORDED_OP_H_

#include "cooking/LinearAllocator.h"

#include <stdint.h>

namespace cooking
{

struct RecordedOp
{
	const int op_id;

	void Replay();

	static void* operator new(size_t size, LinearAllocator& allocator) {
		return allocator.alloc<char>(size);
	}
	static void operator delete(void* ptr, LinearAllocator& allocator) {}

protected:
	RecordedOp(unsigned int op_id) : op_id(op_id) {}

}; // RecordedOp

enum RecordedOpType
{
	// status
	CHANGE_SHADER_OP = 0,
	FLUSH_SHADER_OP,
	RENDER_CLEAR_OP,
	// draw
	DRAW_QUAD_OP,

	RECORDED_OP_COUNT,
};

/************************************************************************/
/* status                                                               */
/************************************************************************/

struct ChangeShaderOp : RecordedOp
{
	ChangeShaderOp(int shader) 
		: RecordedOp(CHANGE_SHADER_OP), shader(shader) {}
	int shader;
}; // ChangeShaderOp

struct FlushShaderOp : RecordedOp
{
	FlushShaderOp() 
		: RecordedOp(FLUSH_SHADER_OP) {}
}; // FlushShaderOp

struct RenderClearOp : RecordedOp
{
	RenderClearOp(uint32_t color) 
		: RecordedOp(RENDER_CLEAR_OP), color(color) {}
	uint32_t color;
}; // RenderClearOp

/************************************************************************/
/* draw                                                                 */
/************************************************************************/

struct DrawQuadOp : RecordedOp
{
	DrawQuadOp(uint32_t col_mul, uint32_t col_add, uint32_t col_rmap, uint32_t col_gmap, uint32_t col_bmap, 
		const float* vertices, const float* texcoords, int tex_id)
		: RecordedOp(DRAW_QUAD_OP), col_mul(col_mul), col_add(col_add), col_rmap(col_rmap), 
		col_gmap(col_gmap), col_bmap(col_bmap), tex_id(tex_id) {
		memcpy(this->vertices, vertices, sizeof(this->vertices));
		memcpy(this->texcoords, texcoords, sizeof(this->texcoords));
	}

	uint32_t col_mul, col_add;
	uint32_t col_rmap, col_gmap, col_bmap;

	float vertices[8], texcoords[8];
	int tex_id;
	
}; // DrawQuadOp

}

#endif // _COOKING_RECORDED_OP_H_