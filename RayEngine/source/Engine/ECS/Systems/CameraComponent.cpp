#include "CameraComponent.h"

#include "Transform.h"
#include "Instance/Instance.h"
#include "Rendering/Debug/Draw.h"

using namespace ECS; 

void SysCameraComponent::Update(EntityID InEntity, CameraComponent& InComponent)
{
    const Transform& t = Get<Transform>(InEntity);
    const CameraInstance c {
        t.GetPosition(),
        t.GetRotation(),
        InComponent.FOV,
        InComponent.Far,
        InComponent.Near };

    // TODO: Camera manager
    auto& i = Engine::Instance::Get();
    auto& s = i.GetRenderScene(); 
    s.SetCamera(c);

    if (i.IsEditor())
        Rendering::DebugSphere(t.GetPosition());
}
