#include "FrameTargets.h"

void Rendering::FrameTargets::Init(const Vec2I &InSize)
{
    RenderTarget::Desc msaaDesc{
        .size = Vec3I(InSize.x, InSize.y, 0),
        .multisample = 4
    };
    
    msaaFrame.Init(msaaDesc);
    msaaNormals.Init(msaaDesc);
    msaaDepth.Init({ 
        .size = Vec3I(InSize.x, InSize.y, 0),
        .format = WGPUTextureFormat_Depth16Unorm,
        .type = TextureType::DEPTH,
        .multisample = 4
    });
    
    frame.Init({ .size = Vec3I(InSize.x, InSize.y, 0) });
    normals.Init({ .size = Vec3I(InSize.x, InSize.y, 0) });
    ssao.Init({ .size = Vec3I(InSize.x, InSize.y, 0) });
}

void Rendering::FrameTargets::Deinit()
{
    msaaFrame.Deinit();
    msaaNormals.Deinit();
    msaaDepth.Deinit();
    
    frame.Deinit();
    normals.Deinit();
    ssao.Deinit();
}
