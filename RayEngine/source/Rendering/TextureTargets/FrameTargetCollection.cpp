#include "FrameTargetCollection.h"

#include "Context/FXConfig.h"

void Rendering::FrameTargetCollection::Init(const Vec2I& InRes, const FXConfig &InFX, bool InCubemap)
{
    Deinit();

    auto type = InCubemap ? TextureType::CUBEMAP : TextureType::TEX;
    auto highFormat = PIXELFORMAT_UNCOMPRESSED_R16G16B16A16;
    auto lowFormat = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    const Vec3I res = {
        InRes.x,
        InRes.y,
        0
    };
    
    depthTex = CreateDepth(InRes, type);
    
    for (auto& target : frameTargets.All())
    {
        target.Setup(res, "TexFrame", highFormat, -1, type);
        target.AttachDepth(depthTex);
    }
    
    for (auto& t : sceneTargets.All())
    {
        if (t.TryBeginSetup(res))
        {
            t.CreateBuffer("TexPosition", highFormat, 1.0f, -1, 1, type);
            t.CreateBuffer("TexNormal", highFormat, 1.0f, -1, 1, type);
            t.CreateBuffer("TexData", highFormat, 1.0f, -1, 1, type);
            t.EndSetup();
            t.AttachDepth(depthTex);
        }
    }

    if (surfaceTarget.TryBeginSetup(res))
    {
        surfaceTarget.CreateBuffer("TexAlbedo", lowFormat, 1.0f, -1, 1, type);
        surfaceTarget.CreateBuffer("TexSurface", lowFormat, 1.0f, -1, 1, type);
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
            target.Setup(aoRes, "TexAO", highFormat, RL_TEXTURE_FILTER_LINEAR);
        }
    }
    
    if (InFX.Bloom && InFX.BloomPasses.Get() >= 2)
    {
        float bloomScale = 1.0f;
        bloomTargets = SwapTarget(InFX.BloomPasses);
        for (auto& target : bloomTargets.All())
        {
            Vec3I bloomRes = (res.To<float>() * bloomScale).To<int>();
            target.Setup(bloomRes, "TexBloom", lowFormat, RL_TEXTURE_FILTER_LINEAR);
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
        rlUnloadTexture(depthTex);
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
