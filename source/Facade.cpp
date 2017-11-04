#include "cooking/Facade.h"
#include "cooking/DisplayList.h"
#include "cooking/DisplayOp.h"
#include "cooking/RenderTask.h"
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

/************************************************************************/
/* draw                                                                 */
/************************************************************************/

void draw_quad(DisplayList* dl, uint32_t col_mul, uint32_t col_add, uint32_t col_rmap, 
			   uint32_t col_gmap, uint32_t col_bmap, const float* vertices, const float* texcoords, int tex_id)
{
	new (dl->AddOp(sizeof(DrawQuadOp))) DrawQuadOp(
		col_mul, col_add, col_rmap, col_gmap, col_bmap, vertices, texcoords, tex_id);
}

}