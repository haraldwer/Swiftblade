#include "RenderTarget.h"

#include "Interface/Textures.h"
#include "Rendering/Resources/Shader.h"
#include "State/State.h"

bool Rendering::RenderTarget::Setup(const Vec3I &InRes, const String &InName, TextureFormat InFormat, TextureParamValue InDefaultFilter, TextureType InType)
{
    if (TryBeginSetup(InRes, InType))
    {
        CreateBuffer(InName, InFormat, 1.0, InDefaultFilter, 1);
        EndSetup();
        return true;
    }
    return false;
}

bool Rendering::RenderTarget::TryBeginSetup(const Vec3I& InRes, TextureType InType)
{
    int targetWidth = Utility::Math::Max(InRes.x, 16);
    int targetHeight = Utility::Math::Max(InRes.y, 16);
    if (size.x == targetWidth &&
        size.y == targetHeight &&
        type == InType)
        return false;
    if (targetWidth <= 0 || targetHeight <= 0)
        return false;

    Unload(); 
    
    size = {
        targetWidth, 
        targetHeight, 
        InRes.z
    };
    type = InType;

    // Create framebuffer
    frameBuffer = RHI::CreateFramebuffer();
    CHECK_ASSERT(!frameBuffer, "Unable to create framebuffer"); 
    RHI::EnableFramebuffer(frameBuffer);

    return true;
}

void Rendering::RenderTarget::EndSetup()
{
    CHECK_RETURN(!frameBuffer);
    
    // Attach to framebuffer
    RHI::ActivateDrawBuffers(static_cast<int>(textures.size())); // Count - Depth
    int i = 0;
    for (TargetTex& buff : textures)
    {
        CHECK_ASSERT(buff.texture, "Texture invalid");
        RHI::FramebufferAttach(buff.texture, type, TextureAttachment::COLOR, i, 1, 0);
        buff.layerFace = 0;
        i++;
    }
    CHECK_ASSERT(!RHI::FramebufferComplete(), "Framebuffer incomplete");
    RHI::DisableFramebuffer();
}

void Rendering::RenderTarget::Unload()
{
    for (const TargetTex& buff : textures)
        RHI::DestroyTexture(buff.texture);
    textures.clear(); 

    if (frameBuffer)
    {
        RHI::DestroyFramebuffer(frameBuffer);
        frameBuffer = 0;
    }
    size = {};
}

void Rendering::RenderTarget::Attach(const int InLayerFace)
{
    RHI::EnableFramebuffer(frameBuffer);
    int i = 0;
    for (TargetTex& buff : textures)
    {
        CHECK_ASSERT(buff.texture, "Texture invalid");
        CHECK_CONTINUE(buff.layerFace == InLayerFace);
        RHI::FramebufferAttach(buff.texture, type, TextureAttachment::COLOR, i, 1, InLayerFace);
        buff.layerFace = InLayerFace;
        i++;
    }
    RHI::DisableFramebuffer();
}

void Rendering::RenderTarget::AttachDepth(const uint32 &InTarget) const
{
    // Maybe assume that 
    RHI::EnableFramebuffer(frameBuffer);
    RHI::FramebufferAttach(InTarget, type, TextureAttachment::DEPTH);
    RHI::DisableFramebuffer();
}

void Rendering::RenderTarget::Bind(ShaderResource &InShader, int &InOutSlot, const TextureParamValue InFilter, const int InLayerFace, const String &InPostfix) const
{
    for (const auto& buff : textures)
    {
        const int loc = InShader.GetLocation(buff.name + InPostfix);
        CHECK_CONTINUE(loc < 0);
        CHECK_ASSERT(!buff.texture, "Tex nullptr");

        InOutSlot++;
        TextureCommand cmd;
        cmd.shaderLoc = loc;
        cmd.filter = InFilter == TextureParamValue::NONE ? buff.defaultFilter : InFilter;
        // Use one layer / side if specified, instead of entire 3D texture / cubemap
        const bool useLayer = InLayerFace >= 0 && type != TextureType::TEXTURE; 
        cmd.type = useLayer ? type : TextureType::TEXTURE;
        cmd.id = useLayer ? buff.views.at(InLayerFace) : buff.texture;
        rlState::current.Set(cmd, InOutSlot);

        const int scaleLoc = InShader.GetLocation(buff.name + InPostfix + "Scale");
        if (scaleLoc >= 0)
        {
            Vec2F scaled = buff.scaledSize.xy.To<float>();
            Vec2F ref = buff.size.xy.To<float>();
            Vec2F scale = scaled / ref;
            RHI::SetUniform(scaleLoc, &scale);
        }
        
        const int texelLoc = InShader.GetLocation(buff.name + InPostfix + "Texel");
        if (texelLoc >= 0)
        {
            Vec2F texel = Vec2F(1.0f) / buff.scaledSize.xy.To<float>();
            RHI::SetUniform(texelLoc, &texel);
        }

        const int sizeLoc = InShader.GetLocation(buff.name + InPostfix + "Size");
        if (sizeLoc >= 0)
        {
            const Vec2F s = buff.scaledSize.xy.To<float>();
            RHI::SetUniform(sizeLoc, &s);
        }
    }
}

void Rendering::RenderTarget::CreateBuffer(const String &InName, TextureFormat InFormat, const float InResScale, const TextureParamValue InDefaultFilter, const int InMips)
{
    const float scale = Utility::Math::Max(1.0f, InResScale);
    const auto scaled = (size.To<float>() * scale).To<int>(); 
    CHECK_RETURN_LOG(scaled.x <= 0 || scaled.y <= 0, "Buffer too small");
    CHECK_RETURN_LOG(scaled.z <= 0 && type == TextureType::TEXTURE_3D, "Buffer too small");

    const Vec3I texSize = scaled;
    
    auto& buffer = textures.emplace_back();
    buffer.name = InName;
    buffer.defaultFilter = InDefaultFilter;
    buffer.size = size;
    buffer.scaledSize = scaled;
    buffer.texture = RHI::CreateTexture(texSize, InFormat, type, InMips);
}
