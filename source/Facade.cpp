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
	dl->SetShaderType(shader);
	new (dl->AddOp(sizeof(ChangeShaderOp))) ChangeShaderOp(shader);
}

void flush_shader(DisplayList* dl)
{
	new (dl->AddOp(sizeof(FlushShaderOp))) FlushShaderOp();
}

void set_render_clear_flag(DisplayList* dl, int flag)
{
	new (dl->AddOp(sizeof(SetRenderClearFlagOp))) SetRenderClearFlagOp(flag);
}

void render_clear(DisplayList* dl, uint32_t color)
{
	new (dl->AddOp(sizeof(RenderClearOp))) RenderClearOp(color);
}

void set_blend(DisplayList* dl, int src, int dst, int func)
{
	new (dl->AddOp(sizeof(SetBlendOp))) SetBlendOp(src, dst, func);
}

void set_shader_blend_mode(DisplayList* dl, int mode)
{
	new (dl->AddOp(sizeof(SetShaderBlendModeOp))) SetShaderBlendModeOp(mode);
}

void set_shader_filter_mode(DisplayList* dl, int mode)
{
	new (dl->AddOp(sizeof(SetShaderFilterModeOp))) SetShaderFilterModeOp(mode);
}

void set_color_sprite(DisplayList* dl, uint32_t col_mul, uint32_t col_add,
	                  uint32_t col_rmap, uint32_t col_gmap, uint32_t col_bmap)
{
	new (dl->AddOp(sizeof(SetColorSprite))) SetColorSprite(col_mul, col_add, col_rmap, col_gmap, col_bmap);
}

void set_color_sprite3(DisplayList* dl, uint32_t col_mul, uint32_t col_add,
	uint32_t col_rmap, uint32_t col_gmap, uint32_t col_bmap)
{
	new (dl->AddOp(sizeof(SetColorSprite3))) SetColorSprite3(col_mul, col_add, col_rmap, col_gmap, col_bmap);
}

void set_color_filter(DisplayList* dl, uint32_t col_mul, uint32_t col_add)
{
	new (dl->AddOp(sizeof(SetColorFilter))) SetColorFilter(col_mul, col_add);
}

/************************************************************************/
/* draw                                                                 */
/************************************************************************/

void draw_quad_sprite(DisplayList* dl, const float* vertices, const float* texcoords, int tex_id)
{
	new (dl->AddOp(sizeof(DrawQuadSpriteOp))) DrawQuadSpriteOp(
		vertices, texcoords, tex_id);
}

void draw_quad_sprite3(DisplayList* dl, const float* vertices, const float* texcoords, int tex_id)
{
	new (dl->AddOp(sizeof(DrawQuadSprite3Op))) DrawQuadSprite3Op(
		vertices, texcoords, tex_id);
}

void draw_quad_filter(DisplayList* dl, const float* vertices, const float* texcoords, int tex_id)
{
	new (dl->AddOp(sizeof(DrawQuadFilterOp))) DrawQuadFilterOp(
		vertices, texcoords, tex_id);
}

void draw_quad_mask(DisplayList* dl, const float* positions, const float* texcoords,
	                const float* texcoords_mask, int tex, int tex_mask)
{
	new (dl->AddOp(sizeof(DrawQuadMaskOp))) DrawQuadMaskOp(
		positions, texcoords, texcoords_mask, tex, tex_mask);
}

/************************************************************************/
/* load                                                                 */
/************************************************************************/

void update_dtex_c2(DisplayList* dl, uint32_t id, int tex_id, int tex_w, int tex_h, const sm::i16_rect& region)
{
	new (dl->AddOp(sizeof(UpdateDTexC2Op))) UpdateDTexC2Op(
		id, tex_id, tex_w, tex_h, region);
}

//void load_glyph(DisplayList* dl, int unicode, const gtxt_glyph_style* gs, uint64_t uid)
//{
//	new (dl->AddOp(sizeof(LoadGlyphOp))) LoadGlyphOp(
//		unicode, gs, uid);
//}

}