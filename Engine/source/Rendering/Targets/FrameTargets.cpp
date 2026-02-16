#include "FrameTargets.h"

void Rendering::FrameTargets::Init(const Vec2I &InSize)
{
    msaaFrame.Init({ 
        .size = Vec3I(InSize.x, InSize.y, 0),
        .multisample = 4
    });
    
    msaaDepth.Init({ 
        .size = Vec3I(InSize.x, InSize.y, 0),
        .format = wgpu::TextureFormat::Depth16Unorm,
        .type = TextureType::DEPTH,
        .multisample = 4
    });
    
    frame.Init({ .size = Vec3I(InSize.x, InSize.y, 0) });
    normals.Init({ .size = Vec3I(InSize.x, InSize.y, 0) });
}

void Rendering::FrameTargets::Deinit()
{
    msaaFrame.Deinit();
    msaaNormals.Deinit();
    msaaDepth.Deinit();
    
    frame.Deinit();
    normals.Deinit();
}
