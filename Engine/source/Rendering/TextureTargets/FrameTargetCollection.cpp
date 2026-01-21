#include "FrameTargetCollection.h"

#include "Context/FXConfig.h"
#include "Interface/Textures.h"

void Rendering::FrameTargetCollection::Init(const Vec2I& InRes, const FXConfig &InFX, bool InCubemap)
{
    Deinit();

    auto type = InCubemap ? TextureType::CUBEMAP : TextureType::TEXTURE;
    auto highFormat = TextureFormat::RGBA16;
    auto lowFormat = TextureFormat::RGBA8;
    const Vec3I res = {
        InRes.x,
        InRes.y,
        0
    };
    
    depthTex = RHI::CreateTextureDepth(InRes, type);
    
    for (auto& target : frameTargets.All())
    {
        target.Setup(res, "TexFrame", highFormat, TextureParamValue::NONE, type);
        target.AttachDepth(depthTex);
    }
    
    for (auto& t : sceneTargets.All())
    {
        if (t.TryBeginSetup(res, type))
        {
            t.CreateBuffer("TexPosition", highFormat);
            t.CreateBuffer("TexNormal", highFormat);
            t.CreateBuffer("TexData", highFormat);
            t.EndSetup();
            t.AttachDepth(depthTex);
        }
    }

    if (surfaceTarget.TryBeginSetup(res, type))
    {
        surfaceTarget.CreateBuffer("TexAlbedo", lowFormat);
        surfaceTarget.CreateBuffer("TexSurface", lowFormat);
        surfaceTarget.EndSetup();
    }
    
    luminTargets.Init(
        InRes, 
        InFX.IrradianceScale, 
        InFX.RadianceScale);

    if (InFX.SSAO)
    {
        for (auto& target : aoTargets.All())
        {
            Vec3I aoRes = (res.To<float>() * InFX.SSAOScale.Get()).To<int>();
            target.Setup(aoRes, "TexAO", highFormat, TextureParamValue::LINEAR);
        }
    }
    
    if (InFX.Bloom && InFX.BloomPasses.Get() >= 2)
    {
        float bloomScale = 1.0f;
        bloomTargets = SwapTarget(InFX.BloomPasses);
        for (auto& target : bloomTargets.All())
        {
            Vec3I bloomRes = (res.To<float>() * bloomScale).To<int>();
            target.Setup(bloomRes, "TexBloom", lowFormat, TextureParamValue::LINEAR);
            bloomScale *= InFX.BloomDownscale;
        }
    }
}

void Rendering::FrameTargetCollection::Deinit()
{
    for (auto& t : sceneTargets.All())
        t.Unload();
    for (auto& t : aoTargets.All())
        t.Unload();
    for (auto& t : frameTargets.All())
        t.Unload();
    for (auto& t : bloomTargets.All())
        t.Unload();
    luminTargets.Deinit();
    if (depthTex != 0)
    {
        RHI::DestroyTexture(depthTex);
        depthTex = 0;
    }
}

OrderedMap<String, Vector<Rendering::RenderTarget::TargetTex>> Rendering::FrameTargetCollection::GetNamed()
{
    OrderedMap<String, Vector<RenderTarget::TargetTex>> result; 
    result["Scene"] = sceneTargets.Curr().GetTextures();
    result["Surface"] = surfaceTarget.GetTextures();
    result["Frame"] = frameTargets.Curr().GetTextures();
    result["AO"] = aoTargets.Curr().GetTextures();
    result["Bloom"] = bloomTargets.Curr().GetTextures(); 
    result["Irradiance"] = luminTargets.irradianceTarget.GetTextures(); 
    result["Radiance"] = luminTargets.radianceTarget.GetTextures(); 
    return result;
}
