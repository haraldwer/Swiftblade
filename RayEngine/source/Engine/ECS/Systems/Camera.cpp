#include "Camera.h"

#include "Transform.h"
#include "Engine/Rendering/Debug/DebugDraw.h"
#include "game/Game.h"
#include "Utility/RayUtility.h"

using namespace ECS; 

void SysCamera::Update(EntityID InEntity, Camera& InComponent, double InDelta)
{
    const Transform& t = Get<Transform>(InEntity);
    const CameraInstance c {
        t.GetPosition(),
        t.GetRotation(),
        InComponent.FOV };

    // TODO: Camera manager
    Engine::InstanceBase::Get().GetRenderScene().SetCamera(c);

    if (Engine::InstanceBase::Get().IsEditor())
        Rendering::DebugSphere(t.GetPosition());
}
