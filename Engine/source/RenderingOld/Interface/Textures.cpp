#include "Textures.h"
uint32 Rendering::RHI::CreateFramebuffer() { return 0; }
void Rendering::RHI::DestroyFramebuffer(uint32 InBuffer) {}
void Rendering::RHI::EnableFramebuffer(uint32 InBuffer) {}
void Rendering::RHI::DisableFramebuffer() {}
void Rendering::RHI::BindFramebuffer(uint32 InBuffer) {}
bool Rendering::RHI::FramebufferComplete() { return false; }
void Rendering::RHI::ActivateDrawBuffers(int InCount) {}

void Rendering::RHI::FramebufferAttach(uint32 InTexture, TextureType InType, TextureAttachment InAttachment, int InSlot,
    int InMip, int InSide) {}

uint32 Rendering::RHI::CreateTexture(const Vec3I &InRes, TextureFormat InFormat, TextureType InType, int InMips) { return 0; }
uint32 Rendering::RHI::CreateTextureView(uint32 InTex, TextureFormat InFormat, int InLayer) { return 0; }
uint32 Rendering::RHI::CreateTextureDepth(const Vec2I &InSize, TextureType InType) { return 0; }
void Rendering::RHI::DestroyTexture(uint32 InTexture) {}
void Rendering::RHI::ActivateTextureSlot(int InSlot) {}
void Rendering::RHI::EnableTexture(uint32 InTex, TextureType InType) {}
void Rendering::RHI::DisableTexture(TextureType InType) {}
void Rendering::RHI::BindTexture(uint32 InTex, TextureType InType) {}
void Rendering::RHI::UnbindTexture(TextureType InType) {}
void Rendering::RHI::SetBlendMode(BlendMode InMode) {}
void Rendering::RHI::SetTextureParam(TextureParam InParam, TextureParamValue InValue, TextureType InType) {}
