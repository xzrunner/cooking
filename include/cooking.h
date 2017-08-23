#ifndef _COOKING_H_
#define _COOKING_H_

#include <stdint.h>

namespace cooking
{

class DisplayList;
class RenderTask;

void change_shader(DisplayList* dl, int shader);

void flush_shader(DisplayList* dl);

void draw_quad(DisplayList* dl, uint32_t col_mul, uint32_t col_add, uint32_t col_rmap, 
			   uint32_t col_gmap, uint32_t col_bmap, const float* vertices, const float* texcoords, int tex_id);

void add_task(RenderTask* task);

void flush();

}

#endif // _COOKING_H_