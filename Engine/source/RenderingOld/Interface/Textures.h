#pragma once

#include "Types.h"

// Render hardware interface 
// OpenGL abstraction
namespace Rendering
{
    namespace RHI
    {
        // Creating frame buffers
        uint32 CreateFramebuffer();
        void DestroyFramebuffer(uint32 InBuffer);
        void EnableFramebuffer(uint32 InBuffer);
        void DisableFramebuffer();
        void BindFramebuffer(uint32 InBuffer);
        bool FramebufferComplete();
        void ActivateDrawBuffers(int InCount);
        void FramebufferAttach(uint32 InTexture, TextureType InType, TextureAttachment InAttachment, int InSlot = 0, int InMip = 1, int InSide = 0);
        
        // Creating textures
        uint32 CreateTexture(const Vec3I& InRes, TextureFormat InFormat = TextureFormat::RGBA16, TextureType InType = TextureType::TEXTURE, int InMips = 1);
        uint32 CreateTextureView(uint32 InTex, TextureFormat InFormat, int InLayer);
        uint32 CreateTextureDepth(const Vec2I& InSize, TextureType InType);
        void DestroyTexture(uint32 InTexture);

        // Enabling / disabling
        void ActivateTextureSlot(int InSlot);
        void EnableTexture(uint32 InTex, TextureType InType);
        void DisableTexture(TextureType InType);
        void BindTexture(uint32 InTex, TextureType InType);
        void UnbindTexture(TextureType InType);
        
        // Writing to textures
        void SetBlendMode(BlendMode InMode);
        
        // Reading from textures
        void SetTextureParam(TextureParam InParam, TextureParamValue InValue, TextureType InType);
    }
}
