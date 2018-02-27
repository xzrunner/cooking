#include "cooking/DisplayOpFunc.h"

#include <unirender/RenderContext.h>
#include <shaderlab/Blackboard.h>
#include <shaderlab/ShaderMgr.h>
#include <shaderlab/FilterShader.h>
#include <shaderlab/Sprite2Shader.h>
#include <shaderlab/Sprite3Shader.h>
#include <shaderlab/BlendShader.h>
#include <shaderlab/MaskShader.h>
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
	sl::Blackboard::Instance()->GetShaderMgr()->SetShader(sl::ShaderType(op.shader));
}

void DisplayOpFunc::ReplayFlushShaderOp(const FlushShaderOp& op)
{
	sl::Blackboard::Instance()->GetShaderMgr()->FlushShader();
}

void DisplayOpFunc::ReplaySetRenderClearFlagOp(const SetRenderClearFlagOp& op)
{
	ur::RenderContext& ur_rc = sl::Blackboard::Instance()->GetShaderMgr()->GetContext();
	ur_rc.SetClearFlag(op.flag);
}

void DisplayOpFunc::ReplayRenderClearOp(const RenderClearOp& op)
{
	ur::RenderContext& ur_rc = sl::Blackboard::Instance()->GetShaderMgr()->GetContext();
	ur_rc.Clear(op.color);
}

void DisplayOpFunc::ReplaySetBlendOp(const SetBlendOp& op)
{
	ur::RenderContext& ur_rc = sl::Blackboard::Instance()->GetShaderMgr()->GetContext();
	ur_rc.SetBlend(op.src, op.dst);
	ur_rc.SetBlendEquation(op.func);
}

void DisplayOpFunc::ReplaySetShaderBlendModeOp(const SetShaderBlendModeOp& op)
{
	auto mgr = sl::Blackboard::Instance()->GetShaderMgr();
	auto shader = static_cast<sl::BlendShader*>(mgr->GetShader(sl::BLEND));
	shader->SetMode(op.mode);
}

void DisplayOpFunc::ReplaySetShaderFilterModeOp(const SetShaderFilterModeOp& op)
{
	auto mgr = sl::Blackboard::Instance()->GetShaderMgr();
	auto shader = static_cast<sl::BlendShader*>(mgr->GetShader(sl::FILTER));
	shader->SetMode(op.mode);
}

void DisplayOpFunc::ReplaySetColorSprite(const SetColorSprite& op)
{
	auto mgr = sl::Blackboard::Instance()->GetShaderMgr();
	auto shader = static_cast<sl::Sprite2Shader*>(mgr->GetShader(sl::SPRITE2));
	shader->SetColor(op.col_mul, op.col_add);
	shader->SetColorMap(op.col_rmap, op.col_gmap, op.col_bmap);
}

void DisplayOpFunc::ReplaySetColorSprite3(const SetColorSprite3& op)
{
	auto mgr = sl::Blackboard::Instance()->GetShaderMgr();
	auto shader = static_cast<sl::Sprite2Shader*>(mgr->GetShader(sl::SPRITE3));
	shader->SetColor(op.col_mul, op.col_add);
	shader->SetColorMap(op.col_rmap, op.col_gmap, op.col_bmap);
}

void DisplayOpFunc::ReplaySetColorFilter(const SetColorFilter& op)
{
	auto mgr = sl::Blackboard::Instance()->GetShaderMgr();
	auto shader = static_cast<sl::FilterShader*>(mgr->GetShader(sl::FILTER));
	shader->SetColor(op.col_mul, op.col_add);
}

/************************************************************************/
/* draw                                                                 */
/************************************************************************/

void DisplayOpFunc::ReplayDrawQuadSpriteOp(const DrawQuadSpriteOp& op)
{
	auto mgr = sl::Blackboard::Instance()->GetShaderMgr();
	auto shader = static_cast<sl::Sprite2Shader*>(mgr->GetShader(sl::SPRITE2));
	shader->DrawQuad(op.vertices, op.texcoords, op.tex_id);
}

void DisplayOpFunc::ReplayDrawQuadSprite3Op(const DrawQuadSprite3Op& op)
{
	auto mgr = sl::Blackboard::Instance()->GetShaderMgr();
	auto shader = static_cast<sl::Sprite3Shader*>(mgr->GetShader(sl::SPRITE3));
	shader->Draw(op.vertices, op.texcoords, op.tex_id);
}

void DisplayOpFunc::ReplayDrawQuadFilterOp(const DrawQuadFilterOp& op)
{
	auto mgr = sl::Blackboard::Instance()->GetShaderMgr();
	auto shader = static_cast<sl::FilterShader*>(mgr->GetShader(sl::FILTER));
	shader->Draw(op.vertices, op.texcoords, op.tex_id);
}

void DisplayOpFunc::ReplayDrawQuadMaskOp(const DrawQuadMaskOp& op)
{
	auto mgr = sl::Blackboard::Instance()->GetShaderMgr();
	auto shader = static_cast<sl::MaskShader*>(mgr->GetShader(sl::MASK));
	shader->Draw(op.positions, op.texcoords, op.texcoords_mask, op.tex, op.tex_mask);
}

/************************************************************************/
/* load                                                                 */
/************************************************************************/

void DisplayOpFunc::ReplayUpdateDTexC2Op(const UpdateDTexC2Op& op)
{
	auto mgr = sl::Blackboard::Instance()->GetShaderMgr();
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