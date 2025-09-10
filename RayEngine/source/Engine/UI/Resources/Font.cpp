#include "Font.h"

#include "Utility/File/File.h"
#include "raylib.h"
#include "ImGui/rlImGui.h"

bool FontResource::Load(const String& InIdentifier)
{
    identifier = InIdentifier;
    
    return true;
}

bool FontResource::Unload()
{
    for (auto& ptr : sizes)
    {
        if (ptr.second)
        {
            UnloadFont(*ptr.second);
            delete (ptr.second);
        }
    }
    sizes.clear();
    return true;
}

Utility::Timepoint FontResource::GetEditTime() const
{
    return Utility::GetFileWriteTime(identifier);  
}

Font* FontResource::Get(const uint32 InSize)
{
    CHECK_ASSERT(InSize == 0, "Invalid size");
    if (sizes.contains(InSize))
        return sizes.at(InSize);
    
    auto& f = sizes[InSize];
    
    f = new Font();
    
    //*f = LoadFontEx(identifier.c_str(), static_cast<int>(InSize), nullptr, 0);
    //SetTextureFilter(f->texture, TEXTURE_FILTER_TRILINEAR);

    int fontSize = static_cast<int>(InSize);
    int fileSize = 0;
    unsigned char *fileData = LoadFileData(identifier.c_str(), &fileSize);
    
    // SDF font generation from TTF font
    f->baseSize = 16;
    f->glyphCount = 95;
    f->glyphPadding = fontSize / 10;
    
    // Parameters > font size: auto, no glyphs array provided (0), glyphs count: 0 (defaults to 95)
    f->glyphs = LoadFontData(
        fileData,
        fileSize,
        fontSize,
        nullptr,
        0,
        FONT_SDF);
    
    // Parameters > glyphs count: 95, font size: auto, glyphs padding in image: auto, pack method: 1 (Skyline algorythm)
    Image atlas = GenImageFontAtlas(
        f->glyphs,
        &f->recs,
        f->glyphCount,
        fontSize,
        f->glyphPadding,
        1);
    f->texture = LoadTextureFromImage(atlas);
    UnloadImage(atlas);
    
    return f;
}

Shader* FontResource::GetShader() const
{
    if (const auto res = sdfShader.Get())
        return res->Get();
    return nullptr;
}

bool FontResource::Edit(const String &InName)
{
    if (ImGui::CollapsingHeader("Loaded sizes"))
    {
        for (auto& s : sizes)
        {
            CHECK_CONTINUE(!s.second)
            ImGui::Text(("Size: " + Utility::ToStr(s.first)).c_str());
            const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
            const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
            const Vec2F size = {vMax.x - vMin.x - 0.1f, vMax.y - vMin.y - 0.1f};
            const float mul = static_cast<float>(s.second->texture.width) / size.width;
            SetTextureWrap(s.second->texture, TEXTURE_WRAP_REPEAT);
            rlImGuiImageRect(
                &s.second->texture,
                static_cast<int>(size.x),
                static_cast<int>(static_cast<float>(s.second->texture.height) / mul),
                Rectangle{
                    0, 0,
                    static_cast<float>(s.second->texture.width),
                    static_cast<float>(-s.second->texture.height)
                });
        }
    }
    return false;
}
