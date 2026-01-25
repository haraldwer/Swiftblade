#include "Particle.h"

#include "Transform.h"
#include "Scene/Culling/VisibilityMask.h"

void ECS::SysParticle::Frame(EntityID InID, Particle& InComponent)
{
    if (!InComponent.Visible)
        return;

    while (InComponent.id == 0)
    {
        // Generate new hash
        const double t = static_cast<double>(InID) + Utility::Time::Get().Total();
        InComponent.id = Utility::Hash(t);  
    }
    
    const Transform& t = Get<Transform>(InID);
    uint8 mask = static_cast<uint8>(Rendering::VisibilityMask::DEFAULT);
    if (InComponent.Lumin)
        mask |= static_cast<uint8>(Rendering::VisibilityMask::LUMIN);
    if (InComponent.Shadows)
        mask |= static_cast<uint8>(Rendering::VisibilityMask::SHADOWS);
        
    // TODO:
    /*
    auto& s = Engine::Instance::Get().GetRenderScene();
    s.AddParticle({
        .particle = InComponent.Resource,
        .material = InComponent.Material,
        .model = InComponent.Model,
        .transform = t.World(),
        .extent = t.GetScale(Transform::Space::WORLD).Length(),
        .id = InComponent.id,
        .mask = mask,
    });
    */
}
