#include "Runtime/Graphics/Shaders/CFluidPlaneShader.hpp"

#include "Runtime/World/CRipple.hpp"
#include "Runtime/World/CRippleManager.hpp"

namespace urde {

void CFluidPlaneShader::PrepareBinding(u32 maxVertCount) {
  CGraphics::CommitResources([&](boo::IGraphicsDataFactory::Context& ctx) {
    m_vbo = ctx.newDynamicBuffer(boo::BufferUse::Vertex, sizeof(Vertex), maxVertCount);
    if (m_pipelines.m_tessellation) {
      m_pvbo = ctx.newDynamicBuffer(boo::BufferUse::Vertex, sizeof(PatchVertex), maxVertCount);
    }
    m_uniBuf = ctx.newDynamicBuffer(boo::BufferUse::Uniform, sizeof(Uniform), 1);

    const std::array<boo::ObjToken<boo::IGraphicsBuffer>, 3> ubufs{{
        m_uniBuf.get(),
        m_uniBuf.get(),
        m_uniBuf.get(),
    }};
    constexpr std::array<boo::PipelineStage, 3> ubufStages{
        boo::PipelineStage::Vertex,
        boo::PipelineStage::Vertex,
        boo::PipelineStage::Fragment,
    };
    constexpr std::array<size_t, 3> ubufOffs{
        0,
        0,
        1280,
    };
    constexpr std::array<size_t, 3> ubufSizes{
        1280,
        1280,
        sizeof(CModelShaders::LightingUniform),
    };

    size_t texCount = 0;
    std::array<boo::ObjToken<boo::ITexture>, 8> texs;
    if (m_patternTex1) {
      texs[texCount++] = m_patternTex1->GetBooTexture();
    }
    if (m_patternTex2) {
      texs[texCount++] = m_patternTex2->GetBooTexture();
    }
    if (m_colorTex) {
      texs[texCount++] = m_colorTex->GetBooTexture();
    }
    if (m_bumpMap) {
      texs[texCount++] = m_bumpMap->GetBooTexture();
    }
    if (m_envMap) {
      texs[texCount++] = m_envMap->GetBooTexture();
    }
    if (m_envBumpMap) {
      texs[texCount++] = m_envBumpMap->GetBooTexture();
    }
    if (m_lightmap) {
      texs[texCount++] = m_lightmap->GetBooTexture();
    }
    auto regular = ctx.newShaderDataBinding(m_pipelines.m_regular, m_vbo.get(), nullptr, nullptr, ubufs.size(),
                                            ubufs.data(), ubufStages.data(), ubufOffs.data(), ubufSizes.data(),
                                            texCount, texs.data(), nullptr, nullptr);
    boo::ObjToken<boo::IShaderDataBinding> tessellation;
    if (m_pipelines.m_tessellation) {
      texs[texCount++] = m_rippleMap.get();
      tessellation = ctx.newShaderDataBinding(m_pipelines.m_tessellation, m_pvbo.get(), nullptr, nullptr, ubufs.size(),
                                              ubufs.data(), ubufStages.data(), ubufOffs.data(), ubufSizes.data(),
                                              texCount, texs.data(), nullptr, nullptr);
    }
    m_dataBind = {regular, tessellation};
    return true;
  } BooTrace);
}

CFluidPlaneShader::CFluidPlaneShader(EFluidType type, const TLockedToken<CTexture>& patternTex1,
                                     const TLockedToken<CTexture>& patternTex2, const TLockedToken<CTexture>& colorTex,
                                     const TLockedToken<CTexture>& bumpMap, const TLockedToken<CTexture>& envMap,
                                     const TLockedToken<CTexture>& envBumpMap, const TLockedToken<CTexture>& lightmap,
                                     const boo::ObjToken<boo::ITextureS>& rippleMap, bool doubleLightmapBlend,
                                     bool additive, u32 maxVertCount)
: m_patternTex1(patternTex1)
, m_patternTex2(patternTex2)
, m_colorTex(colorTex)
, m_bumpMap(bumpMap)
, m_envMap(envMap)
, m_envBumpMap(envBumpMap)
, m_lightmap(lightmap)
, m_rippleMap(rippleMap) {
  SFluidPlaneShaderInfo shaderInfo(type, m_patternTex1.HasReference(), m_patternTex2.HasReference(),
                                   m_colorTex.HasReference(), m_bumpMap.HasReference(), m_envMap.HasReference(),
                                   m_envBumpMap.HasReference(), m_lightmap.HasReference(), m_rippleMap.operator bool(),
                                   doubleLightmapBlend, additive);
  m_pipelines = _cache.GetOrBuildShader(shaderInfo);
  PrepareBinding(maxVertCount);
}

CFluidPlaneShader::CFluidPlaneShader(const TLockedToken<CTexture>& patternTex1,
                                     const TLockedToken<CTexture>& patternTex2, const TLockedToken<CTexture>& colorTex,
                                     u32 maxVertCount)
: m_patternTex1(patternTex1), m_patternTex2(patternTex2), m_colorTex(colorTex) {
  SFluidPlaneDoorShaderInfo shaderInfo(m_patternTex1.HasReference(), m_patternTex2.HasReference(),
                                       m_colorTex.HasReference());
  m_pipelines = _cache.GetOrBuildShader(shaderInfo);
  PrepareBinding(maxVertCount);
}

void CFluidPlaneShader::prepareDraw(const RenderSetupInfo& info) {
  Uniform& uni = *reinterpret_cast<Uniform*>(m_uniBuf->map(sizeof(Uniform)));
  uni.m_mv = CGraphics::g_GXModelView.toMatrix4f();
  uni.m_mvNorm = info.normMtx;
  uni.m_proj = CGraphics::GetPerspectiveProjectionMatrix(true);
  uni.m_texMtxs = info.texMtxs;
  uni.m_lighting.ActivateLights(info.lights);
  for (size_t i = 0; i < uni.m_lighting.colorRegs.size(); ++i) {
    uni.m_lighting.colorRegs[i] = info.kColors[i];
  }
  uni.m_lighting.mulColor = info.kColors[3];
  uni.m_lighting.fog = CGraphics::g_Fog;
  uni.m_pad2.x() = info.indScale;
  m_uniBuf->unmap();
}

void CFluidPlaneShader::prepareDraw(const RenderSetupInfo& info, const zeus::CVector3f& waterCenter,
                                    const CRippleManager& rippleManager, const zeus::CColor& colorMul,
                                    float rippleNormResolution) {
  Uniform& uni = *reinterpret_cast<Uniform*>(m_uniBuf->map(sizeof(Uniform)));
  uni.m_mv = CGraphics::g_GXModelView.toMatrix4f();
  uni.m_mvNorm = info.normMtx;
  uni.m_proj = CGraphics::GetPerspectiveProjectionMatrix(true);
  uni.m_texMtxs = info.texMtxs;

  size_t i = 0;
  for (const CRipple& ripple : rippleManager.GetRipples()) {
    assert(i < uni.m_ripple.size() && "Too many ripples");
    Ripple& rOut = uni.m_ripple[i++];
    if (ripple.GetTime() >= ripple.GetTimeFalloff()) {
      rOut.center.zeroOut();
      rOut.params.zeroOut();
      continue;
    }
    zeus::CVector3f localPos = ripple.GetCenter() - waterCenter;
    rOut.center.x() = float(localPos.x());
    rOut.center.y() = float(localPos.y());
    rOut.center.z() = ripple.GetTime() * ripple.GetOOTimeFalloff();
    rOut.center.w() = ripple.GetOODistanceFalloff();
    rOut.params.x() = ripple.GetAmplitude();
    rOut.params.y() = ripple.GetPhase();
    rOut.params.z() =
        (1.f - ripple.GetTime() * ripple.GetOOTimeFalloff() * ripple.GetOOTimeFalloff()) * ripple.GetFrequency();
  }
  uni.m_colorMul = colorMul;
  uni.m_pad[0].x() = rippleNormResolution;
  uni.m_lighting.ActivateLights(info.lights);
  for (i = 0; i < uni.m_lighting.colorRegs.size(); ++i) {
    uni.m_lighting.colorRegs[i] = info.kColors[i];
  }
  uni.m_lighting.mulColor = info.kColors[3];
  uni.m_lighting.fog = CGraphics::g_Fog;
  uni.m_pad2.x() = info.indScale;
  m_uniBuf->unmap();
}

void CFluidPlaneShader::loadVerts(const std::vector<Vertex>& verts, const std::vector<PatchVertex>& pVerts) {
  m_vbo->load(verts.data(), verts.size() * sizeof(Vertex));
  if (m_pvbo)
    m_pvbo->load(pVerts.data(), pVerts.size() * sizeof(PatchVertex));
}

} // namespace urde
