#include "cooking/RecordedOp.h"

#include <shaderlab/ShaderMgr.h>
#include <shaderlab/FilterShader.h>
#include <shaderlab/Sprite2Shader.h>

namespace cooking
{

static void 
change_shader_relpay(void* op)
{
	ChangeShaderOp* cs = static_cast<ChangeShaderOp*>(op);
	sl::ShaderMgr::Instance()->SetShader(sl::ShaderType(cs->shader));
}

static void 
flush_shader_relpay(void* op)
{
	sl::ShaderMgr::Instance()->FlushShader();
}

static void 
draw_quad_relpay(void* op)
{
	DrawQuadOp* dq = static_cast<DrawQuadOp*>(op);

	sl::ShaderMgr* mgr = sl::ShaderMgr::Instance();
	if (mgr->GetShaderType() == sl::FILTER) {
		sl::FilterShader* shader = static_cast<sl::FilterShader*>(mgr->GetShader(sl::FILTER));
		shader->SetColor(dq->col_mul, dq->col_add);
		shader->Draw(dq->vertices, dq->texcoords, dq->tex_id);
	} else if (mgr->GetShaderType() == sl::SPRITE2) {
		sl::Sprite2Shader* shader = static_cast<sl::Sprite2Shader*>(mgr->GetShader(sl::SPRITE2));
		shader->SetColor(dq->col_mul, dq->col_add);
		shader->SetColorMap(dq->col_rmap, dq->col_gmap, dq->col_bmap);
		shader->DrawQuad(dq->vertices, dq->texcoords, dq->tex_id);
	}
}

typedef void (*Replay)(void* op);
Replay REPLAY_FUNCS[RecordedOpType::RECORDED_OP_COUNT] = 
{
	change_shader_relpay,
	flush_shader_relpay,
	draw_quad_relpay,
};

void RecordedOp::Replay()
{
	REPLAY_FUNCS[op_id](this);
}

}