#include "Camera.h"

#include "Transform.h"
#include "game/Game.h"

using namespace ECS; 

void SysCamera::Update(EntityID InEntity, Camera& InComponent, double InDelta)
{
    const Transform& t = Get<Transform>(InEntity);
    const CameraInstance c {
        t.GetPosition(),
        t.GetRotation(),
        InComponent.FOV };
    Game::Get().GetRenderScene().SetCamera(c);
}
