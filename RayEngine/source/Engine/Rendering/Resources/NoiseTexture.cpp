#include "NoiseTexture.h"

#include <FastNoiseLite/FastNoiseLite.h>
#include <external/stb_image_write.h>

#include "Utility/File/File.h"

bool NoiseTextureResource::Load(const String& InPath)
{
    Tex = ResTexture(InPath + ".png");
    Identifier = InPath;
    PropertyOwnerBase::Load(InPath);
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
        if (TextureResource* res = Tex.Get())
        {
            if (Texture* tex = res->Get())
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

void NoiseTextureResource::Generate()
{
    int res = Resolution;
    Color* data = new Color[res * res];

    if (static_cast<int>(Type.Get()) >= static_cast<int>(NoiseType::COUNT) ||
        static_cast<int>(Type.Get()) < 0)
        Type = 0;

    Generate(data, res);
    
    // Cache file
    String filename = Tex.Identifier();
    int channels = 4;
    int result = stbi_write_png(filename.c_str(), res, res, channels, data, channels * res);
    if (result == 1)
        LOG("Noise texture saved: " + filename);

    // Hot reloading handles the tex
}

// Map coord to torus, creating a 3D vector that wraps around
static Vec3F TorusMapping(const Vec2F& InPos)
{
    float c = 3.0f, a = 1.0f; // torus parameters (controlling size)
    float xt = (c + a * cosf(2.0f * PI_FLOAT * InPos.y)) * cosf(2.0f * PI_FLOAT * InPos.x);
    float yt = (c + a * cosf(2.0f * PI_FLOAT * InPos.y)) * sinf(2.0f * PI_FLOAT * InPos.x);
    float zt = a * sinf(2.0f * PI_FLOAT * InPos.y);
    return { xt, yt, zt };
}

void NoiseTextureResource::Generate(Color* InData, const int InResolution)
{
    FastNoiseLite noise;
    noise.SetNoiseType(static_cast<FastNoiseLite::NoiseType>(Type.Get()));
    noise.SetFrequency(Frequency.Get());
    noise.SetSeed(Seed.Get());

    std::function get = [&](int InX, int InY)
    {
        Vec2F n = Vec2F(
                static_cast<float>(InX),
                static_cast<float>(InY)    
            ) / static_cast<float>(InResolution);
        Vec3F pos = TorusMapping(n);
        float p = noise.GetNoise(pos.x, pos.y, pos.z);
        p = Utility::Math::Clamp(p, -1.0f, 1.0f);
        p = (p + 1.0f) / 2.0f;
        return static_cast<uint8>(p * 255.0f);
    };

    std::function for_every = [&](const std::function<void(int, int)>& InFunc)
    {
        for (int y = 0; y < InResolution; y++)
            for (int x = 0; x < InResolution; x++)
                InFunc(x, y);
    };

    switch (static_cast<NoiseType>(Type.Get()))
    {
    default:
        for_every([&](int x, int y) {
            const uint8 i = get(x, y); 
            InData[y * InResolution + x] = Color(i, 0, 0, 255);
        });
        noise.SetSeed(Seed.Get() + 1);
        for_every([&](int x, int y) {
            InData[y * InResolution + x].g = get(x, y); 
        });
        noise.SetSeed(Seed.Get() + 2);
        for_every([&](int x, int y) {
            InData[y * InResolution + x].b = get(x, y);
        });
        break;
    case NoiseType::CELLULAR:
        for_every([&](const int x, const int y) {
            const uint8 i = get(x, y); 
            InData[y * InResolution + x] = Color(i, 0, 0, 255);
        });
        noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);
        for_every([&](const int x, const int y) {
            InData[y * InResolution + x].g = get(x, y);
        });
        break;
    }
}
