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
    ImGui::SameLine();
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
            const float minSize = size.x * 0.5f; 
            
            // Send to ImGui
            rlImGuiImageRect(
                tex,
                static_cast<int>(minSize),
                static_cast<int>(minSize),
                Rectangle{ 0,0,
                    static_cast<float>(tex->width),
                    -static_cast<float>(tex->height)
                });

            ImGui::SameLine();
            rlImGuiImageRect(
                tex,
                static_cast<int>(minSize),
                static_cast<int>(minSize),
                Rectangle{ 0,0,
                    static_cast<float>(tex->width),
                    -static_cast<float>(tex->height)
                });

            rlImGuiImageRect(
                tex,
                static_cast<int>(minSize),
                static_cast<int>(minSize),
                Rectangle{ 0,0,
                    static_cast<float>(tex->width),
                    -static_cast<float>(tex->height)
                });

            ImGui::SameLine();
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
        result |= PropertyOwnerBase::Edit(InName, InOffset);
        if (ImGui::Button("Generate"))
            Generate();
        ImGui::SameLine();
        if (ImGui::Button("Save"))
            PropertyOwnerBase::Save(Identifier);
        ImGui::SameLine();
        if (ImGui::Button("Close"))
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
    static Utility::Timer timer;
    auto& properties = Perlin.Get();

    
    std::function<void(Vec2F InPos)> noise = [&]()
    {
        float c=4, a=1; // torus parameters (controlling size)
        float xt = (c+a*cosf(2*PI_FLOAT*InPos.y))*cosf(2*PI_FLOAT*nx);
        float yt = (c+a*cosf(2*PI_FLOAT*InPos.y))*sinf(2*PI_FLOAT*nx);
        float zt = a*sinf(2*PI_FLOAT*ny);
            
        float p = stb_perlin_fbm_noise3(xt, yt, zt,
            properties.Lacunarity,
            properties.Gain,
            properties.Octaves);
            
        p = Utility::Math::Clamp(p, -1.0f, 1.0f);
        p = (p + 1.0f) / 2.0f;
    };
    
    for (int y = 0; y < InResolution; y++)
    {
        for (int x = 0; x < InResolution; x++)
        {
            // See raylib GenImagePerlinNoise()
            float nx = static_cast<float>(x) * Scale.Get().x / InResolution;
            float ny = static_cast<float>(y) * Scale.Get().y / InResolution;

            nx += timer.Ellapsed();
            ny += timer.Ellapsed();

            

            uint8 intensity = static_cast<uint8>(p * 255.0f);
            InData[y * InResolution + x] = { .r = intensity, .g = intensity, .b = intensity, .a = 255};
        }
    }
}
