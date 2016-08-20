#include "Graphics/CGraphics.hpp"
#include "CCameraFilter.hpp"
#include "GameGlobalObjects.hpp"
#include "Graphics/CBooRenderer.hpp"
#include "CSimplePool.hpp"

namespace urde
{

void CCameraFilterPass::DrawFilter(EFilterType type, EFilterShape shape, const zeus::CColor& color,
                                   const CTexture* tex, float uvScale)
{
    switch (type)
    {
    case EFilterType::Multiply:
    case EFilterType::Invert:
    case EFilterType::Add:
    case EFilterType::Subtract:
    case EFilterType::Blend:
    case EFilterType::SceneAdd:
    case EFilterType::NoColor:
    case EFilterType::Passthru:
    case EFilterType::Widescreen:
    default: return;
    }
    DrawFilterShape(shape, color, tex, uvScale);
}

void CCameraFilterPass::DrawFilterShape(EFilterShape shape, const zeus::CColor& color,
                                        const CTexture* tex, float uvScale)
{
}

void CCameraBlurPass::Draw()
{
    if (x10_curType == EBlurType::NoBlur)
        return;

    SClipScreenRect rect = {};
    rect.xc_width = CGraphics::g_ViewportResolution.x;
    rect.x10_height = CGraphics::g_ViewportResolution.y;
    CGraphics::ResolveSpareTexture(rect);

    if (x10_curType == EBlurType::Xray)
    {
        float blurX = x1c_curValue * g_tweakGui->GetXrayBlurScaleLinear() * 0.25f;
        float blurY = x1c_curValue * g_tweakGui->GetXrayBlurScaleQuadratic() * 0.25f;

        for (int i=0 ; i<4 ; ++i)
        {
            float iflt = i;
            float uvScale = (1.f - (blurX * iflt + blurY * iflt * iflt));
            float uvOffset = uvScale * -0.5f + 0.5f;
        }
    }
}

void CCameraBlurPass::Update(float dt)
{
    if (x28_remainingTime > 0.f)
    {
        x28_remainingTime = std::max(x28_remainingTime - dt, 0.f);
        x1c_curValue = x18_endValue + (x20_startValue - x18_endValue) * x28_remainingTime / x24_totalTime;

        if (x28_remainingTime != 0.f)
            return;

        x10_curType = x14_endType;
    }
}

void CCameraBlurPass::SetBlur(EBlurType type, float amount, float duration)
{
    if (duration == 0.f)
    {
        x24_totalTime = 0.f;
        x28_remainingTime = 0.f;
        x18_endValue = amount;
        x1c_curValue = amount;
        x20_startValue = amount;

        if (x10_curType == EBlurType::NoBlur)
        {
            if (type == EBlurType::Xray)
                x0_paletteTex = g_SimplePool->GetObj("TXTR_XRayPalette");
        }

        x14_endType = type;
        x10_curType = type;
        //x2c_usePersistent = b1;
    }
    else
    {
        //x2c_usePersistent = b1;
        x24_totalTime = duration;
        x28_remainingTime = duration;
        x18_endValue = x1c_curValue;
        x20_startValue = amount;

        if (type != x14_endType)
        {
            if (x10_curType == EBlurType::NoBlur)
            {
                if (type == EBlurType::Xray)
                    x0_paletteTex = g_SimplePool->GetObj("TXTR_XRayPalette");
                x10_curType = type;
            }
            x14_endType = type;
        }
    }
}

void CCameraBlurPass::DisableBlur(float duration)
{
    SetBlur(EBlurType::NoBlur, 0.f, duration);
}

}
