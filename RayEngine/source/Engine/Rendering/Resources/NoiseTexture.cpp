#include "NoiseTexture.h"

#include <external/stb_perlin.h>

bool NoiseTextureResource::Load(const String& InPath)
{
    Tex = ResTexture("Gen_" + InPath);
    Identifier = InPath;
    if (PropertyOwnerBase::Load(InPath))
        Generate();
    return true;
}

bool NoiseTextureResource::Unload()
{
    Tex = ResTexture();
    return PropertyOwnerBase::Unload();
}

Utility::Timepoint NoiseTextureResource::GetEditTime() const
{
    return Utility::GetFileWriteTime(Identifier);
}

ResTexture NoiseTextureResource::Get() const
{
    return Tex;
}

bool NoiseTextureResource::Edit(const String& InName, uint32 InOffset)
{
    if (ImGui::Button("Edit"))
        EditorOpen = !EditorOpen;
    if (!EditorOpen)
        return false;
    
    bool result = false;
    if (ImGui::Begin("Noise Generator"))
    {
        if (auto& tex = GetTex())
        {
            // Adjust size
            const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
            const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
            const Vec2F size = { vMax.x - vMin.x, vMax.y - vMin.y };
            const float minSize = size.x; // Utility::Math::Min(size.x, size.y);
            
            // Send to ImGui
            rlImGuiImageRect(
                tex,
                static_cast<int>(minSize),
                static_cast<int>(minSize),
                Rectangle{ 0,0,
                    static_cast<float>(tex->width),
                    -static_cast<float>(tex->height)
                });
        }
        
        ImGui::Separator();
        result |= PropertyOwnerBase::Edit(InName);
        if (result)
            Generate();
        if (ImGui::Button("Save"))
            PropertyOwnerBase::Save(Identifier);
        if (ImGui::Button("Save and close"))
        {
            EditorOpen = false;
            result = true;
        }
        ImGui::End();
    }
    return result;
}

Texture*& NoiseTextureResource::GetTex() const
{
    TextureResource* res = Tex.Get();
    CHECK_ASSERT(!res, "Texture resource invalid");
    return res->Ptr;
}

void NoiseTextureResource::Generate()
{
    int res = Resolution;
    Color* data = new Color[res * res];

    if (static_cast<int>(Type.Get()) >= static_cast<int>(NoiseType::COUNT))
        Type = 0;
    
    switch (static_cast<NoiseType>(Type.Get()))
    {
    case NoiseType::PERLIN:
    default:
        GeneratePerlin(data, res);
        break;
    }

    Image image = {
        .data = data,
        .width = res,
        .height = res,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    auto& tex = GetTex();
    if (tex)
        UnloadTexture(*tex);
    tex = new Texture();
    *tex = LoadTextureFromImage(image);
    UnloadImage(image);
}

void NoiseTextureResource::GeneratePerlin(Color* InData, const int InResolution)
{
    auto& properties = Perlin.Get();
    for (int y = 0; y < InResolution; y++)
    {
        for (int x = 0; x < InResolution; x++)
        {
            // See raylib GenImagePerlinNoise()
            float nx = static_cast<float>(x) * InScale.Get().x / static_cast<float>(InResolution);
            float ny = static_cast<float>(y) * InScale.Get().x / static_cast<float>(InResolution);
            
            float p = stb_perlin_fbm_noise3(nx, ny, 1.0f,
                properties.Lacunarity,
                properties.Gain,
                properties.Octaves);
            p = Utility::Math::Clamp(p, -1.0f, 1.0f);
            p = (p + 1.0f) / 2.0f;

            uint8 intensity = static_cast<uint8>(p * 255.0f);
            InData[y * InResolution + x] = { .r = intensity, .g = intensity, .b = intensity, .a = 255};
        }
    }
}
