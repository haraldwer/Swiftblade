#include "Camera.h"

#include "Transform.h"
#include "Engine/Rendering/Debug/Draw.h"
#include "game/GameInstance.h"
#include "Utility/RayUtility.h"

using namespace ECS; 

void SysCamera::Update(EntityID InEntity, Camera& InComponent, double InDelta)
{
    const Transform& t = Get<Transform>(InEntity);
    const CameraInstance c {
        t.GetPosition(),
        t.GetRotation(),
        InComponent.FOV,
        InComponent.Far,
        InComponent.Near };

    // TODO: Camera manager
    Engine::Instance::Get().GetRenderScene().SetCamera(c);

    if (Engine::Instance::Get().IsEditor())
        Rendering::DebugSphere(t.GetPosition());
}
