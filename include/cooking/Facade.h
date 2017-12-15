#ifndef _COOKING_FACADE_H_
#define _COOKING_FACADE_H_

// update_dtex_c2
#include <SM_Rect.h>

// load_glyph
#include <gtxt/gtxt_glyph.h>

#include <stdint.h>

namespace cooking
{

class DisplayList;

/************************************************************************/
/* status                                                               */
/************************************************************************/

void change_shader(DisplayList* dl, int shader);

void flush_shader(DisplayList* dl);

void set_blend(DisplayList* dl, int src, int dst, int func);

/************************************************************************/
/* draw                                                                 */
/************************************************************************/

void draw_quad(DisplayList* dl, uint32_t col_mul, uint32_t col_add, uint32_t col_rmap, 
			   uint32_t col_gmap, uint32_t col_bmap, const float* vertices, const float* texcoords, int tex_id);

/************************************************************************/
/* load                                                                 */
/************************************************************************/

void update_dtex_c2(DisplayList* dl, uint32_t id, int tex_id, int tex_w, int tex_h, const sm::i16_rect& region);

void load_glyph(DisplayList* dl, int unicode, const gtxt_glyph_style* gs, uint64_t uid);

}

#endif // _COOKING_FACADE_H_