#include "CameraComponent.h"

#include "Transform.h"
#include "Engine/Rendering/Debug/Draw.h"
#include "game/GameInstance.h"
#include "Utility/RayUtility.h"

using namespace ECS; 

void SysCameraComponent::Update(EntityID InEntity, CameraComponent& InComponent, double InDelta)
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
    s.SetTime(Utility::Time::Get().Total());

    if (i.IsEditor())
        Rendering::DebugSphere(t.GetPosition());
}
