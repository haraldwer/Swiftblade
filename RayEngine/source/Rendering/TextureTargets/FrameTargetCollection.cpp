#include "FrameTargetCollection.h"

#include "raylib.h"

void Rendering::FrameTargetCollection::Init(const RenderTexture& InTarget)
{
    Deinit();
    
    if (SceneTarget.TryBeginSetup(InTarget))
    {
        SceneTarget.CreateBuffer("TexPosition", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
        SceneTarget.CreateBuffer("TexNormal", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
        SceneTarget.CreateBuffer("TexColor", PIXELFORMAT_UNCOMPRESSED_R4G4B4A4);
        SceneTarget.CreateBuffer("TexVelocity", PIXELFORMAT_UNCOMPRESSED_R16G16B16);
        SceneTarget.EndSetup(InTarget);
    }

    for (auto& target : SSAOTargets.All())
        target.Setup(InTarget, "TexAO", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    for (auto& target : FireTargets.All())
        target.Setup(InTarget, "TexFire", PIXELFORMAT_UNCOMPRESSED_R16G16B16);
    FrameTarget.Setup(InTarget, "TexFrame", PIXELFORMAT_UNCOMPRESSED_R8G8B8);
    QuantizeTarget.Setup(InTarget, "TexQuantize", PIXELFORMAT_UNCOMPRESSED_R8G8B8);
}

void Rendering::FrameTargetCollection::Deinit()
{
    SceneTarget.Unload();
    for (auto& t : SSAOTargets.All())
        t.Unload();
    for (auto& t : FireTargets.All())
        t.Unload();
    FrameTarget.Unload();
    QuantizeTarget.Unload();
}

Map<String, Vector<RenderTarget::Buffer>> Rendering::FrameTargetCollection::GetBuffers()
{
    Map<String, Vector<RenderTarget::Buffer>> result; 
    result["Scene"] = SceneTarget.GetBuffers();
    result["Fire"] = FireTargets.Curr().GetBuffers();
    result["SSAO"] = SSAOTargets.Curr().GetBuffers();
    result["Quantize"] = QuantizeTarget.GetBuffers();
    result["Frame"] = FrameTarget.GetBuffers();
    return result;
}
