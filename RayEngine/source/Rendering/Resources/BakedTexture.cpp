#include "BakedTexture.h"

#include "raylib.h"
#include "Pipeline/Renderer.h"
#include "State/State.h"

bool Rendering::BakedTexture::Load()
{
    PropertyFile::Load();
    
    tex = new RenderTexture();
    CHECK_ASSERT(!tex, "Failed to setup baked texture");
    *tex = LoadRenderTexture(data.Res, data.Res);
    return true;
}

bool Rendering::BakedTexture::Unload()
{
    baked = false;
    if (tex)
    {
        UnloadRenderTexture(*tex);
        delete tex;
        tex = nullptr;
    }
    return true;
}

Utility::Timepoint Rendering::BakedTexture::GetEditTime() const
{
    if (auto s = data.Shader.Get().Get())
        return s->GetEditTime();
    return PropertyFile::GetEditTime();
}

void Rendering::BakedTexture::Bind(const String& InName, ShaderResource& InShader, int& InOutSlot, int InFilter) const
{
    CHECK_RETURN(!baked);
    
    CHECK_ASSERT(!tex, "Tex nullptr");
    
    const int loc = InShader.GetLocation(InName);
    CHECK_RETURN(loc < 0);

    InOutSlot++;
    TextureCommand cmd;
    cmd.shaderLoc = loc;
    cmd.id = tex->id;
    cmd.filter = InFilter;
    rlState::current.Set(cmd, InOutSlot);
}

bool Rendering::BakedTexture::Bake()
{
    baked = Renderer::Bake(*this);
    return baked;
}
