#include "cooking/ReplayOpBuilder.h"

#include <unirender/UR_RenderContext.h>
#include <shaderlab/ShaderMgr.h>
#include <shaderlab/FilterShader.h>
#include <shaderlab/Sprite2Shader.h>

#include <algorithm>

namespace cooking
{

#define OP_RECEIVER(Type) \
        [](const DisplayOp& op) { ReplayOpBuilder::Replay##Type(static_cast<const Type&>(op)); },
void ReplayOpBuilder::Replay(const std::vector<DisplayOp*>& ops, int begin, int end)
{
	typedef void(*OpDispatcher) (const DisplayOp& op);
	static OpDispatcher receivers[] = BUILD_OP_LUT(OP_RECEIVER);

	if (ops.empty()) {
		return;
	}

	int sz = ops.size();
	if (begin < 0) {
		begin = 0;
	}
	if (end < 0) {
		end = sz;
	}
	begin = std::min(begin, sz);
	end = std::min(end, sz);

	if (begin >= end || begin >= sz) {
		return;
	}

	DisplayOp*const* ptr = &ops[begin];
	for (int i = 0, n = end - begin; i < n; ++i, ++ptr) {
		DisplayOp* op = *ptr;
		receivers[op->id](*op);
	}
}

/************************************************************************/
/* status                                                               */
/************************************************************************/

void ReplayOpBuilder::ReplayChangeShaderOp(const ChangeShaderOp& op)
{
	sl::ShaderMgr::Instance()->SetShader(sl::ShaderType(op.shader));
}

void ReplayOpBuilder::ReplayFlushShaderOp(const FlushShaderOp& op)
{
	sl::ShaderMgr::Instance()->FlushShader();
}

void ReplayOpBuilder::ReplayRenderClearOp(const RenderClearOp& op)
{
	sl::ShaderMgr::Instance()->GetContext()->Clear(op.color);
}

/************************************************************************/
/* draw                                                                 */
/************************************************************************/

void ReplayOpBuilder::ReplayDrawQuadOp(const DrawQuadOp& op)
{
	sl::ShaderMgr* mgr = sl::ShaderMgr::Instance();
	if (mgr->GetShaderType() == sl::FILTER) {
		sl::FilterShader* shader = static_cast<sl::FilterShader*>(mgr->GetShader(sl::FILTER));
		shader->SetColor(op.col_mul, op.col_add);
		shader->Draw(op.vertices, op.texcoords, op.tex_id);
	} else if (mgr->GetShaderType() == sl::SPRITE2) {
		sl::Sprite2Shader* shader = static_cast<sl::Sprite2Shader*>(mgr->GetShader(sl::SPRITE2));
		shader->SetColor(op.col_mul, op.col_add);
		shader->SetColorMap(op.col_rmap, op.col_gmap, op.col_bmap);
		shader->DrawQuad(op.vertices, op.texcoords, op.tex_id);
	}
}

}