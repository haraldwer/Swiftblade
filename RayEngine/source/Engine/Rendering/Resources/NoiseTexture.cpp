#include "NoiseTexture.h"

#include <external/stb_perlin.h>

bool NoiseTextureResource::Load(const String& InPath)
{
    // Load config file
    Identifier = InPath;
    bool result = PropertyOwnerBase::Load(InPath);
    Generate();
    return result;
}

bool NoiseTextureResource::Unload()
{
    if (Tex)
    {
        UnloadTexture(*Tex);
        Tex = nullptr;
    }
    return true;
}

Utility::Timepoint NoiseTextureResource::GetEditTime() const
{
    return Utility::GetFileWriteTime(Identifier);
}

Texture2D* NoiseTextureResource::Get() const
{
    return Tex;
}

bool NoiseTextureResource::Edit(const String& InName)
{
    if (ImGui::Button("Edit"))
        EditorOpen = !EditorOpen;
    if (!EditorOpen)
        return false;
    
    bool result = false;
    if (ImGui::Begin("Noise Generator"))
    {
        if (Tex)
        {
            // Adjust size
            const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
            const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
            const Vec2F size = { vMax.x - vMin.x, vMax.y - vMin.y };
            const float minSize = Utility::Math::Min(size.x, size.y);
            
            // Send to ImGui
            rlImGuiImageRect(
                Tex,
                static_cast<int>(minSize),
                static_cast<int>(minSize),
                Rectangle{ 0,0,
                    static_cast<float>(Tex->width),
                    -static_cast<float>(Tex->height)
                });
        }
        
        ImGui::Separator();
        result |= PropertyOwnerBase::Edit(InName);
        if (result)
            Generate();
        if (ImGui::Button("Save and close"))
        {
            EditorOpen = false;
            result = true;
        }
        ImGui::End();
    }
    return result;
}

void NoiseTextureResource::Generate()
{
    int res = Resolution;
    Color* data = new Color[res * res];
    GeneratePerlin(data, res);

    Image image = {
        .data = data,
        .width = res,
        .height = res,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    Tex = new Texture2D();
    *Tex = LoadTextureFromImage(image);
    UnloadImage(image);
}

void NoiseTextureResource::GeneratePerlin(Color* InData, const int InResolution)
{
    for (int y = 0; y < InResolution; y++)
    {
        for (int x = 0; x < InResolution; x++)
        {
            // See raylib GenImagePerlinNoise()
            float nx = static_cast<float>(x) * InScale.Get().x;
            float ny = static_cast<float>(y) * InScale.Get().x;
            
            float p = stb_perlin_fbm_noise3(nx, ny, 1.0f, 2.0f, 0.5f, 6);
            p = Utility::Math::Clamp(p, -1.0f, 1.0f);
            p = (p + 1.0f) / 2.0f;

            uint8 intensity = static_cast<uint8>(p * 255.0f);
            InData[y * InResolution + x] = { .r = intensity, .g = intensity, .b = intensity, .a = 255};
        }
    }
}
