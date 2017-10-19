#ifndef _COOKING_DISPLAY_OP_H_
#define _COOKING_DISPLAY_OP_H_

#include <cstdint>

namespace cooking
{

#define MAP_OPS_BASED_ON_TYPE(RENDER_OP_FN) \
	RENDER_OP_FN(ChangeShaderOp) \
	RENDER_OP_FN(FlushShaderOp) \
	RENDER_OP_FN(RenderClearOp) \
	\
	RENDER_OP_FN(DrawQuadOp) \

#define NULLPTR_OP_FN(Type) nullptr,

#define BUILD_OP_LUT(OP_FN) \
        { MAP_OPS_BASED_ON_TYPE(OP_FN) }

#define IDENTITY_FN(Type) Type,
namespace DispalyOpId {
    enum {
        MAP_OPS_BASED_ON_TYPE(IDENTITY_FN)
        Count,
    };
}
static_assert(DispalyOpId::ChangeShaderOp == 0,
        "First index must be zero for LUTs to work");

#define SUPER(Type) DisplayOp(DispalyOpId::Type)

struct DisplayOp
{
	const int id;

protected:
	DisplayOp(unsigned int id) : id(id) {}

}; // DisplayOp

/************************************************************************/
/* status                                                               */
/************************************************************************/

struct ChangeShaderOp : DisplayOp
{
	ChangeShaderOp(int shader)
		: SUPER(ChangeShaderOp)
		, shader(shader) {}
	int shader;
}; // ChangeShaderOp

struct FlushShaderOp : DisplayOp
{
	FlushShaderOp()
		: SUPER(FlushShaderOp) {}
}; // FlushShaderOp

struct RenderClearOp : DisplayOp
{
	RenderClearOp(uint32_t color)
		: SUPER(RenderClearOp)
		, color(color) {}
	uint32_t color;
}; // RenderClearOp

/************************************************************************/
/* draw                                                                 */
/************************************************************************/

struct DrawQuadOp : DisplayOp
{
	DrawQuadOp(uint32_t col_mul, uint32_t col_add, uint32_t col_rmap, uint32_t col_gmap, uint32_t col_bmap,
		const float* vertices, const float* texcoords, int tex_id)
		: SUPER(DrawQuadOp)
		, col_mul(col_mul)
		, col_add(col_add)
		, col_rmap(col_rmap)
		, col_gmap(col_gmap)
		, col_bmap(col_bmap)
		, tex_id(tex_id)
	{
		memcpy(this->vertices, vertices, sizeof(this->vertices));
		memcpy(this->texcoords, texcoords, sizeof(this->texcoords));
	}

	uint32_t col_mul, col_add;
	uint32_t col_rmap, col_gmap, col_bmap;

	float vertices[8], texcoords[8];
	int   tex_id;

}; // DrawQuadOp

}

#endif // _COOKING_DISPLAY_OP_H_