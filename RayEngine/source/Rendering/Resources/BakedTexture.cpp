#include "BakedTexture.h"

#include "raylib.h"
#include "Pipeline/Renderer.h"

bool Rendering::BakedTexture::Load()
{
    PropertyFile::Load();

    bool result = target.Setup(Vec2I(data.Res), data.Name, PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
    CHECK_RETURN_LOG(!result, "Failed to setup baked texture", false);
    return true;
}

bool Rendering::BakedTexture::Unload()
{
    baked = false;
    target.Unload();
    return true;
}

Utility::Timepoint Rendering::BakedTexture::GetEditTime() const
{
    if (auto s = data.Shader.Get().Get())
        return s->GetEditTime();
    return PropertyFile::GetEditTime();
}

bool Rendering::BakedTexture::Bake()
{
    baked = Renderer::Bake(*this);
    return baked;
}
