#include "FrameTargetCollection.h"

#include "raylib.h"
#include "rlgl.h"
#include "Context/FXConfig.h"

void Rendering::FrameTargetCollection::Init(const RenderTexture& InTarget, const FXConfig &InFX)
{
    Deinit();

    const Vec2I res = {
            InTarget.texture.width,
        InTarget.texture.height
    };
    
    for (auto& t : sceneTargets.All())
    {
        if (t.TryBeginSetup(res))
        {
            t.CreateBuffer("TexPosition", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
            t.CreateBuffer("TexNormal", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
            t.CreateBuffer("TexData", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
            t.EndSetup();
            t.AttachDepth(InTarget);
        }
    }

    if (surfaceTarget.TryBeginSetup(res))
    {
        surfaceTarget.CreateBuffer("TexAlbedo", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
        surfaceTarget.CreateBuffer("TexSurface", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
        surfaceTarget.EndSetup();
    }
    
    for (auto& target : frameTargets.All())
        target.Setup(res, "TexFrame", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);

    for (auto& target : aoTargets.All())
    {
        Vec2I aoRes = (res.To<float>() * InFX.SSAOScale.Get()).To<int>();
        target.Setup(aoRes, "TexAO", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, RL_TEXTURE_FILTER_LINEAR);
    }

    if (InFX.BloomPasses.Get() >= 2)
    {
        float bloomScale = 1.0f;
        bloomTargets = SwapTarget(InFX.BloomPasses);
        for (auto& target : bloomTargets.All())
        {
            Vec2I bloomRes = (res.To<float>() * bloomScale).To<int>();
            target.Setup(bloomRes, "TexBloom", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, RL_TEXTURE_FILTER_LINEAR);
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
}

OrderedMap<String, Vector<Rendering::RenderTarget::TargetTex>> Rendering::FrameTargetCollection::GetNamed()
{
    OrderedMap<String, Vector<RenderTarget::TargetTex>> result; 
    result["Scene"] = sceneTargets.Curr().GetTextures();
    result["Surface"] = surfaceTarget.GetTextures();
    result["Frame"] = frameTargets.Curr().GetTextures();
    result["AO"] = aoTargets.Curr().GetTextures();
    result["Bloom"] = bloomTargets.Curr().GetTextures(); 
    return result;
}
