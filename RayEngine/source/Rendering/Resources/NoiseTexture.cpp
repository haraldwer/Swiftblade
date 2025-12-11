#include "NoiseTexture.h"

#include <FastNoiseLite/FastNoiseLite.h>
#include <external/stb_image_write.h>

#include "ImGui/imgui.h"
#include "ImGui/rlImGui.h"
#include "Utility/File/File.h"

bool Rendering::NoiseTextureResource::Load()
{
    if (!Utility::File::Exists(GetCachePath()))
        Generate();
    tex = ResTexture(GetCachePath());
    PropertyFile::Load();
    return true;
}

bool Rendering::NoiseTextureResource::Unload()
{
    tex = ResTexture();
    return PropertyFile::Unload();
}

ResTexture Rendering::NoiseTextureResource::Get() const
{
    return tex;
}

bool Rendering::NoiseTextureResource::Accept(const String &InPath)
{
    return InPath.ends_with(".json") && Utility::File::Name(InPath).starts_with("T_");
}

#ifdef IMGUI_ENABLE

bool Rendering::NoiseTextureResource::Edit(const String& InName, const uint32 InOffset)
{
    ImGui::SameLine();
    if (Utility::Button("Edit##" + InName, InOffset))
        editorOpen = !editorOpen;
    if (!editorOpen)
        return false;
    
    bool result = false;
    if (ImGui::Begin((String("Noise Generator##") + InName).c_str()))
    {
        if (const TextureResource* res = tex.Get())
        {
            if (const Texture* texPtr = res->Get())
            {
                // Adjust size
                const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
                const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
                const Vec2F size = { vMax.x - vMin.x, vMax.y - vMin.y };
                const float minSize = size.x * 0.5f; 
            
                // Send to ImGui
                rlImGuiImageRect(
                    texPtr,
                    static_cast<int>(minSize),
                    static_cast<int>(minSize),
                    Rectangle{ 0,0,
                        static_cast<float>(texPtr->width),
                        -static_cast<float>(texPtr->height)
                    });

                ImGui::SameLine();
                rlImGuiImageRect(
                    texPtr,
                    static_cast<int>(minSize),
                    static_cast<int>(minSize),
                    Rectangle{ 0,0,
                        static_cast<float>(texPtr->width),
                        -static_cast<float>(texPtr->height)
                    });

                rlImGuiImageRect(
                    texPtr,
                    static_cast<int>(minSize),
                    static_cast<int>(minSize),
                    Rectangle{ 0,0,
                        static_cast<float>(texPtr->width),
                        -static_cast<float>(texPtr->height)
                    });

                ImGui::SameLine();
                rlImGuiImageRect(
                    texPtr,
                    static_cast<int>(minSize),
                    static_cast<int>(minSize),
                    Rectangle{ 0,0,
                        static_cast<float>(texPtr->width),
                        -static_cast<float>(texPtr->height)
                    });
            }
        }
        
        ImGui::Separator();
        result |= PropertyFile::Edit(InName, InOffset);
        if (ImGui::Button("Generate"))
            Generate();
        ImGui::SameLine();
        if (ImGui::Button("Save"))
            PropertyFile::Save();
        ImGui::SameLine();
        if (ImGui::Button("Close"))
        {
            editorOpen = false;
            result = true;
        }
        ImGui::End();
    }
    return result;
}

#else

bool Rendering::NoiseTextureResource::Edit(const String& InName, const uint32 InOffset) { return false; }

#endif

void Rendering::NoiseTextureResource::Generate()
{
    const int res = data.Resolution;
    Color* arr = new Color[res * res];

    if (data.Type.Get() >= static_cast<int>(NoiseType::COUNT) ||
        data.Type.Get() < 0)
        data.Type = 0;

    Generate(arr, res);
    
    // Cache file
    const String filename = GetCachePath();
    Utility::File::CreateDir(filename);
    
    const int channels = 4;
    const int result = stbi_write_png(filename.c_str(), res, res, channels, arr, channels * res);
    if (result == 1)
        LOG("Noise texture saved: " + filename);

    delete[] arr;
    arr = nullptr;

    // Hot reloading handles the tex
}

// Map coord to torus, creating a 3D vector that wraps around
static Vec3F TorusMapping(const Vec2F& InPos)
{
    const float c = 3.0f, a = 1.0f; // torus parameters (controlling size)
    float xt = (c + a * cosf(2.0f * PI_FLOAT * InPos.y)) * cosf(2.0f * PI_FLOAT * InPos.x);
    float yt = (c + a * cosf(2.0f * PI_FLOAT * InPos.y)) * sinf(2.0f * PI_FLOAT * InPos.x);
    float zt = a * sinf(2.0f * PI_FLOAT * InPos.y);
    return { xt, yt, zt };
}

void Rendering::NoiseTextureResource::Generate(Color* InData, const int InResolution)
{
    FastNoiseLite noise;
    noise.SetNoiseType(static_cast<FastNoiseLite::NoiseType>(data.Type.Get()));
    noise.SetFrequency(data.Frequency.Get());
    noise.SetSeed(data.Seed.Get());

    const std::function get = [&](const int InX, const int InY)
    {
        const Vec2F n = Vec2F(
                static_cast<float>(InX),
                static_cast<float>(InY)    
            ) / static_cast<float>(InResolution);
        const Vec3F pos = TorusMapping(n);
        float p = noise.GetNoise(pos.x, pos.y, pos.z);
        p = Utility::Math::Clamp(p, -1.0f, 1.0f);
        p = (p + 1.0f) / 2.0f;
        return static_cast<uint8>(p * 255.0f);
    };

    const std::function forEvery = [&](const std::function<void(int, int)>& InFunc)
    {
        for (int y = 0; y < InResolution; y++)
            for (int x = 0; x < InResolution; x++)
                InFunc(x, y);
    };

    switch (static_cast<NoiseType>(data.Type.Get()))
    {
    default:
        forEvery([&](const int InX, const int InY) {
            const uint8 i = get(InX, InY); 
            InData[InY * InResolution + InX] = Color(i, 0, 0, 255);
        });
        noise.SetSeed(data.Seed.Get() + 1);
        forEvery([&](const int InX, const int InY) {
            InData[InY * InResolution + InX].g = get(InX, InY); 
        });
        noise.SetSeed(data.Seed.Get() + 2);
        forEvery([&](const int InX, const int InY) {
            InData[InY * InResolution + InX].b = get(InX, InY);
        });
        break;
    case NoiseType::CELLULAR:
        forEvery([&](const int InX, const int InY) {
            const uint8 i = get(InX, InY); 
            InData[InY * InResolution + InX] = Color(i, 0, 0, 255);
        });
        noise.SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);
        forEvery([&](const int InX, const int InY) {
            InData[InY * InResolution + InX].g = get(InX, InY);
        });
        break;
    }
}

String Rendering::NoiseTextureResource::GetCachePath() const
{
    return Utility::File::GetCachePath(id.Str(), ".png"); 
}
