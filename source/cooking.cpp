#include "cooking.h"

#include "cooking/DisplayList.h"
#include "cooking/RecordedOp.h"
#include "cooking/RenderTask.h"
#include "cooking/Callback.h"

namespace cooking
{

void change_shader(DisplayList* dl, int shader)
{
	dl->AddOp(new (dl->GetAlloc()) ChangeShaderOp(shader));
}

void flush_shader(DisplayList* dl)
{
	dl->AddOp(new (dl->GetAlloc()) FlushShaderOp());	
}

void draw_quad(DisplayList* dl, uint32_t col_mul, uint32_t col_add, uint32_t col_rmap, 
			   uint32_t col_gmap, uint32_t col_bmap, const float* vertices, const float* texcoords, int tex_id)
{
	dl->AddOp(new (dl->GetAlloc()) DrawQuadOp(col_mul, col_add, col_rmap, col_gmap, col_bmap, vertices, texcoords, tex_id));	
}

void add_task(RenderTask* task)
{
	Callback::SubmitTask(task);
}

void flush()
{
	RenderTaskMgr::Instance()->Flush();
}

}