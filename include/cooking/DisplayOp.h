#ifndef _COOKING_DISPLAY_OP_H_
#define _COOKING_DISPLAY_OP_H_

// UpdateDTexC2Op
#include <SM_Rect.h>

// LoadGlyphOp
#include <gtxt/gtxt_glyph.h>
#include <gum/ResourceUID.h>

#include <cstdint>
#include <cstring>

namespace cooking
{

#define MAP_OPS_BASED_ON_TYPE(RENDER_OP_FN) \
	RENDER_OP_FN(ChangeShaderOp) \
	RENDER_OP_FN(FlushShaderOp) \
	RENDER_OP_FN(RenderClearOp) \
	RENDER_OP_FN(SetBlendOp) \
	\
	RENDER_OP_FN(DrawQuadOp) \
	\
	RENDER_OP_FN(UpdateDTexC2Op) \
	RENDER_OP_FN(LoadGlyphOp) \

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

struct SetBlendOp : DisplayOp
{
	SetBlendOp(int src, int dst, int func)
		: SUPER(SetBlendOp)
		, src(src)
		, dst(dst)
		, func(func) {}
	int src, dst;
	int func;
}; // SetBlendOp

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

/************************************************************************/
/* load                                                                 */
/************************************************************************/

struct UpdateDTexC2Op : DisplayOp
{
	UpdateDTexC2Op(uint32_t id, int tex_id,
		int tex_w, int tex_h, const sm::i16_rect& region)
		: SUPER(UpdateDTexC2Op)
		, id(id)
		, tex_id(tex_id)
		, tex_w(tex_w)
		, tex_h(tex_h)
		, region(region)
	{
	}
	
	uint32_t id;

	int tex_id;
	int tex_w, tex_h;

	sm::i16_rect region;

}; // UpdateDTexC2Op

struct LoadGlyphOp : DisplayOp
{
	LoadGlyphOp(int unicode, const gtxt_glyph_style* gs, gum::UID uid)
		: SUPER(LoadGlyphOp)
		, unicode(unicode)
		, gs(*gs)
		, uid(uid)
	{
	}

	int unicode;
	gtxt_glyph_style gs;

	gum::UID uid;

}; // LoadGlyphOp

}

#endif // _COOKING_DISPLAY_OP_H_