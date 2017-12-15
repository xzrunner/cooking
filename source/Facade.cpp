#include "cooking/Facade.h"
#include "cooking/DisplayList.h"
#include "cooking/DisplayOp.h"
#include "cooking/Callback.h"

#include <memory>

namespace cooking
{

/************************************************************************/
/* status                                                               */
/************************************************************************/

void change_shader(DisplayList* dl, int shader)
{
	new (dl->AddOp(sizeof(ChangeShaderOp))) ChangeShaderOp(shader);
}

void flush_shader(DisplayList* dl)
{
	new (dl->AddOp(sizeof(FlushShaderOp))) FlushShaderOp();
}

void set_blend(DisplayList* dl, int src, int dst, int func)
{
	new (dl->AddOp(sizeof(SetBlendOp))) SetBlendOp(src, dst, func);
}

/************************************************************************/
/* draw                                                                 */
/************************************************************************/

void draw_quad(DisplayList* dl, uint32_t col_mul, uint32_t col_add, uint32_t col_rmap, 
			   uint32_t col_gmap, uint32_t col_bmap, const float* vertices, const float* texcoords, int tex_id)
{
	new (dl->AddOp(sizeof(DrawQuadOp))) DrawQuadOp(
		col_mul, col_add, col_rmap, col_gmap, col_bmap, vertices, texcoords, tex_id);
}

/************************************************************************/
/* load                                                                 */
/************************************************************************/

void update_dtex_c2(DisplayList* dl, uint32_t id, int tex_id, int tex_w, int tex_h, const sm::i16_rect& region)
{
	new (dl->AddOp(sizeof(UpdateDTexC2Op))) UpdateDTexC2Op(
		id, tex_id, tex_w, tex_h, region);
}

void load_glyph(DisplayList* dl, int unicode, const gtxt_glyph_style* gs, uint64_t uid)
{
	new (dl->AddOp(sizeof(LoadGlyphOp))) LoadGlyphOp(
		unicode, gs, uid);
}

}