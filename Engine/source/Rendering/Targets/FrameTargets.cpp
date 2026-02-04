#include "FrameTargets.h"

void Rendering::FrameTargets::Init(const Vec2I &InSize)
{
    frame.Init({ Vec3I(InSize.x, InSize.y, 0) });
    
    depth.Init({ 
        .size=Vec3I(InSize.x, InSize.y, 0),
        .format=wgpu::TextureFormat::Depth16Unorm,
        .type=TextureType::DEPTH
    });
}

void Rendering::FrameTargets::Deinit()
{
    frame.Deinit();
    depth.Deinit();
}
