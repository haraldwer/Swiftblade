#include "Texture.h"

#include "Utility/File/File.h"

bool Rendering::TextureResource::Load()
{
    if (id.Str().starts_with("Gen_"))
        return true;
    /*
    ptr = new Texture();
    ptr = LoadTexture(id.Str().c_str());
    */
    // TODO: Load tex
    return true;
}

bool Rendering::TextureResource::Unload()
{
    //if (ptr)
    //{
    //    UnloadTexture(*ptr);
    //    delete (ptr);
    //    ptr = nullptr; 
    //    return true;
    //}
    return false; 
}

Utility::Timepoint Rendering::TextureResource::GetEditTime() const
{
    if (id.Str().starts_with("Gen_"))
        return {};
    return Utility::File::GetWriteTime(id.Str());  
}

bool Rendering::TextureResource::Accept(const String &InPath)
{
    return InPath.ends_with(".png");
}

#ifdef IMGUI_ENABLE

bool Rendering::TextureResource::Edit(const String &InName, uint32 InOffset)
{
    /*
    if (!ptr)
    {
        ImGui::Text("Invalid texture");
        return false;
    }

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
    
    switch (ptr->format)
    {
        case PIXELFORMAT_UNCOMPRESSED_GRAYSCALE:    ImGui::Text("Format: PIXELFORMAT_UNCOMPRESSED_GRAYSCALE"); break;
        case PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA:   ImGui::Text("Format: PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA"); break;
        case PIXELFORMAT_UNCOMPRESSED_R5G6B5:       ImGui::Text("Format: PIXELFORMAT_UNCOMPRESSED_R5G6B5"); break;
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8:       ImGui::Text("Format: PIXELFORMAT_UNCOMPRESSED_R8G8B8"); break;
        case PIXELFORMAT_UNCOMPRESSED_R5G5B5A1:     ImGui::Text("Format: PIXELFORMAT_UNCOMPRESSED_R5G5B5A1"); break;
        case PIXELFORMAT_UNCOMPRESSED_R4G4B4A4:     ImGui::Text("Format: PIXELFORMAT_UNCOMPRESSED_R4G4B4A4"); break;
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8A8:     ImGui::Text("Format: PIXELFORMAT_UNCOMPRESSED_R8G8B8A8"); break;
        case PIXELFORMAT_UNCOMPRESSED_R32:          ImGui::Text("Format: PIXELFORMAT_UNCOMPRESSED_R32"); break;
        case PIXELFORMAT_UNCOMPRESSED_R32G32B32:    ImGui::Text("Format: PIXELFORMAT_UNCOMPRESSED_R32G32B32"); break;
        case PIXELFORMAT_UNCOMPRESSED_R32G32B32A32: ImGui::Text("Format: PIXELFORMAT_UNCOMPRESSED_R32G32B32A32"); break;
        case PIXELFORMAT_UNCOMPRESSED_R16:          ImGui::Text("Format: PIXELFORMAT_UNCOMPRESSED_R16"); break;
        case PIXELFORMAT_UNCOMPRESSED_R16G16B16:    ImGui::Text("Format: PIXELFORMAT_UNCOMPRESSED_R16G16B16"); break;
        case PIXELFORMAT_UNCOMPRESSED_R16G16B16A16: ImGui::Text("Format: PIXELFORMAT_UNCOMPRESSED_R16G16B16A16"); break;
        case PIXELFORMAT_COMPRESSED_DXT1_RGB:       ImGui::Text("Format: PIXELFORMAT_COMPRESSED_DXT1_RGB"); break;
        case PIXELFORMAT_COMPRESSED_DXT1_RGBA:      ImGui::Text("Format: PIXELFORMAT_COMPRESSED_DXT1_RGBA"); break;
        case PIXELFORMAT_COMPRESSED_DXT3_RGBA:      ImGui::Text("Format: PIXELFORMAT_COMPRESSED_DXT3_RGBA"); break;
        case PIXELFORMAT_COMPRESSED_DXT5_RGBA:      ImGui::Text("Format: PIXELFORMAT_COMPRESSED_DXT5_RGBA"); break;
        case PIXELFORMAT_COMPRESSED_ETC1_RGB:       ImGui::Text("Format: PIXELFORMAT_COMPRESSED_ETC1_RGB"); break;
        case PIXELFORMAT_COMPRESSED_ETC2_RGB:       ImGui::Text("Format: PIXELFORMAT_COMPRESSED_ETC2_RGB"); break;
        case PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA:  ImGui::Text("Format: PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA"); break;
        case PIXELFORMAT_COMPRESSED_PVRT_RGB:       ImGui::Text("Format: PIXELFORMAT_COMPRESSED_PVRT_RGB"); break;
        case PIXELFORMAT_COMPRESSED_PVRT_RGBA:      ImGui::Text("Format: PIXELFORMAT_COMPRESSED_PVRT_RGBA"); break;
        case PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA:  ImGui::Text("Format: PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA"); break;
        case PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA:  ImGui::Text("Format: PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA"); break;
        default: ImGui::Text("Unknown format"); break;
    }
    
    ImGui::Text("Size: %ix%i", ptr->width, ptr->height);
    ImGui::Text("Mips: %i", ptr->mipmaps);
    
    */
    return false;
}

#else

bool Rendering::TextureResource::Edit(const String &InName, uint32 InOffset) { return false; }

#endif
