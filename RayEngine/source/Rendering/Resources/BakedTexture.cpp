#include "BakedTexture.h"

#include "raylib.h"
#include "ImGui/rlImGui.h"
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

bool Rendering::BakedTexture::Edit(const String &InName, uint32 InOffset)
{
    bool result = PropertyFile::Edit(InName, InOffset);

    for (auto& tex : target.GetTextures())
    {
        ImGui::Text("Name: %s", tex.name.c_str());

        CHECK_CONTINUE(!tex.tex);
        ImGui::Text("Size: %ix%i", tex.tex->width, tex.tex->height);
        
        const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
        const Vec2F size = {vMax.x - vMin.x - 0.1f, vMax.y - vMin.y - 0.1f};
        const float mul = static_cast<float>(tex.tex->width) / size.width;
        SetTextureWrap(*tex.tex, TEXTURE_WRAP_REPEAT);
        rlImGuiImageRect(
            tex.tex,
            static_cast<int>(size.x),
            static_cast<int>(static_cast<float>(tex.tex->height) / mul),
            Rectangle{
                0, 0,
                static_cast<float>(tex.tex->width),
                static_cast<float>(-tex.tex->height)
            });
    }
    
    return result; 
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

bool Rendering::BakedTexture::Accept(const String &InPath)
{
    return InPath.ends_with(".json") && Utility::File::Name(InPath).starts_with("BT_");
}
