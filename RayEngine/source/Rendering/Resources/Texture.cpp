#include "Texture.h"

#include "Utility/File/File.h"
#include "raylib.h"
#include "ImGui/rlImGui.h"

bool Rendering::TextureResource::Load()
{
    if (id.Str().starts_with("Gen_"))
        return true;
    ptr = new Texture();
    *ptr = LoadTexture(id.Str().c_str());
    return ptr->id;
}

bool Rendering::TextureResource::Unload()
{
    if (ptr)
    {
        UnloadTexture(*ptr);
        delete (ptr);
        ptr = nullptr; 
        return true;
    }
    return false; 
}

Utility::Timepoint Rendering::TextureResource::GetEditTime() const
{
    if (id.Str().starts_with("Gen_"))
        return {};
    return Utility::GetFileWriteTime(id.Str());  
}

bool Rendering::TextureResource::Edit(const String &InName, uint32 InOffset)
{
    if (!ptr)
    {
        ImGui::Text("Invalid texture");
        return false;
    }
    ImGui::Text(("Resolution: " + Utility::ToStr(Vec2I(ptr->width, ptr->height))).c_str());
    ImGui::Text(("Format: " + Utility::ToStr(ptr->format)).c_str());
    ImGui::Text(("MipMaps: " + Utility::ToStr(ptr->mipmaps)).c_str());
    const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
    const Vec2F size = {vMax.x - vMin.x - 0.1f, vMax.y - vMin.y - 0.1f};
    const float mul = static_cast<float>(ptr->width) / size.width;
    SetTextureWrap(*ptr, TEXTURE_WRAP_REPEAT);
    rlImGuiImageRect(
        ptr,
        static_cast<int>(size.x),
        static_cast<int>(static_cast<float>(ptr->height) / mul),
        Rectangle{
            0, 0,
            static_cast<float>(ptr->width),
            static_cast<float>(-ptr->height)
        });
    return false;
}
