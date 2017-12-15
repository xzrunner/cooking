#include "cooking/DisplayOpFunc.h"

#include <unirender/RenderContext.h>
#include <shaderlab/ShaderMgr.h>
#include <shaderlab/FilterShader.h>
#include <shaderlab/Sprite2Shader.h>
#include <gum/DTexC2Strategy.h>
#include <gum/DTex.h>

#include <algorithm>

namespace cooking
{

#define OP_RECEIVER(Type) \
        [](const DisplayOp& op) { DisplayOpFunc::Replay##Type(static_cast<const Type&>(op)); },
void DisplayOpFunc::Replay(const DisplayOp* op)
{
	typedef void(*OpDispatcher) (const DisplayOp& op);
	static OpDispatcher receivers[] = BUILD_OP_LUT(OP_RECEIVER);

	receivers[op->id](*op);
}

#define OP_SIZE(Type) \
		case DispalyOpId::Type: return sizeof(Type);
size_t DisplayOpFunc::Size(const DisplayOp& op)
{
	switch (op.id)
	{
		MAP_OPS_BASED_ON_TYPE(OP_SIZE)
	}
	return 0;
}

/************************************************************************/
/* status                                                               */
/************************************************************************/

void DisplayOpFunc::ReplayChangeShaderOp(const ChangeShaderOp& op)
{
	sl::ShaderMgr::Instance()->SetShader(sl::ShaderType(op.shader));
}

void DisplayOpFunc::ReplayFlushShaderOp(const FlushShaderOp& op)
{
	sl::ShaderMgr::Instance()->FlushShader();
}

void DisplayOpFunc::ReplayRenderClearOp(const RenderClearOp& op)
{
	sl::ShaderMgr::Instance()->GetContext()->Clear(op.color);
}

void DisplayOpFunc::ReplaySetBlendOp(const SetBlendOp& op)
{
	ur::RenderContext* rctx = sl::ShaderMgr::Instance()->GetContext();
	rctx->SetBlend(op.src, op.dst);
	rctx->SetBlendEquation(op.func);
}

/************************************************************************/
/* draw                                                                 */
/************************************************************************/

void DisplayOpFunc::ReplayDrawQuadOp(const DrawQuadOp& op)
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

/************************************************************************/
/* load                                                                 */
/************************************************************************/

void DisplayOpFunc::ReplayUpdateDTexC2Op(const UpdateDTexC2Op& op)
{
	sl::ShaderMgr* mgr = sl::ShaderMgr::Instance();
	sl::ShaderType type = mgr->GetShaderType();
	if (gum::DTexC2Strategy::Instance()->OnC2QueryFail(
		op.id, op.tex_id, op.tex_w, op.tex_h, op.region)) {
		mgr->SetShader(type);
	}
}

void DisplayOpFunc::ReplayLoadGlyphOp(const LoadGlyphOp& op)
{
	struct gtxt_glyph_layout layout;
	uint32_t* bmp = gtxt_glyph_get_bitmap(op.unicode, &op.gs, &layout);
	if (!bmp) {
		return;
	}
	int w = static_cast<int>(layout.sizer.width);
	int h = static_cast<int>(layout.sizer.height);
	gum::DTex::Instance()->LoadGlyph(bmp, w, h, op.uid);
}

}