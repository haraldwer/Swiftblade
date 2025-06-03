#include "FrameTargetCollection.h"

#include "raylib.h"

void Rendering::FrameTargetCollection::Init(const RenderTexture& InTarget)
{
    Deinit();

    for (auto& t : sceneTargets.All())
    {
        if (t.TryBeginSetup(InTarget))
        {
            t.CreateBuffer("TexPosition", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
            t.CreateBuffer("TexNormal", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
            t.CreateBuffer("TexColor", PIXELFORMAT_UNCOMPRESSED_R4G4B4A4);
            t.CreateBuffer("TexData", PIXELFORMAT_UNCOMPRESSED_R16G16B16);
            t.EndSetup(InTarget);
        }
    }

    for (auto& target : aoTargets.All())
        target.Setup(InTarget, "TexAO", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    for (auto& target : fireTargets.All())
        target.Setup(InTarget, "TexFire", PIXELFORMAT_UNCOMPRESSED_R16G16B16);
    for (auto& target : frameTargets.All())
        target.Setup(InTarget, "TexFrame", PIXELFORMAT_UNCOMPRESSED_R8G8B8);
}

void Rendering::FrameTargetCollection::Deinit()
{
    for (auto& t : sceneTargets.All())
        t.Unload();
    for (auto& t : aoTargets.All())
        t.Unload();
    for (auto& t : fireTargets.All())
        t.Unload();
    for (auto& t : frameTargets.All())
        t.Unload();
}

OrderedMap<String, Vector<Rendering::RenderTarget::TargetTex>> Rendering::FrameTargetCollection::GetNamed()
{
    OrderedMap<String, Vector<RenderTarget::TargetTex>> result; 
    result["Scene"] = sceneTargets.Curr().GetTextures();
    result["Fire"] = fireTargets.Curr().GetTextures();
    result["SSAO"] = aoTargets.Curr().GetTextures();
    result["Frame"] = frameTargets.Curr().GetTextures();
    return result;
}
