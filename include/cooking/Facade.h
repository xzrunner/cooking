#ifndef _COOKING_FACADE_H_
#define _COOKING_FACADE_H_

// update_dtex_c2
#include <SM_Rect.h>

#include <stdint.h>

struct gtxt_glyph_style;

namespace cooking
{

class DisplayList;

/************************************************************************/
/* status                                                               */
/************************************************************************/

void change_shader(DisplayList* dl, int shader);

void flush_shader(DisplayList* dl);

void set_render_clear_flag(DisplayList* dl, int flag);
void render_clear(DisplayList* dl, uint32_t color);

void set_blend(DisplayList* dl, int src, int dst, int func);

void set_shader_blend_mode(DisplayList* dl, int mode);
void set_shader_filter_mode(DisplayList* dl, int mode);

void set_color_sprite(DisplayList* dl, uint32_t col_mul, uint32_t col_add, 
	uint32_t col_rmap, uint32_t col_gmap, uint32_t col_bmap);
void set_color_sprite3(DisplayList* dl, uint32_t col_mul, uint32_t col_add,
	uint32_t col_rmap, uint32_t col_gmap, uint32_t col_bmap);
void set_color_filter(DisplayList* dl, uint32_t col_mul, uint32_t col_add);

/************************************************************************/
/* draw                                                                 */
/************************************************************************/

void draw_quad_sprite(DisplayList* dl, const float* vertices, const float* texcoords, int tex_id);

void draw_quad_sprite3(DisplayList* dl, const float* vertices, const float* texcoords, int tex_id);

void draw_quad_filter(DisplayList* dl, const float* vertices, const float* texcoords, int tex_id);

void draw_quad_mask(DisplayList* dl, const float* positions, const float* texcoords,
	                const float* texcoords_mask, int tex, int tex_mask);

/************************************************************************/
/* load                                                                 */
/************************************************************************/

void update_dtex_c2(DisplayList* dl, uint32_t id, int tex_id, int tex_w, int tex_h, const sm::i16_rect& region);

void load_glyph(DisplayList* dl, int unicode, const gtxt_glyph_style* gs, uint64_t uid);

}

#endif // _COOKING_FACADE_H_