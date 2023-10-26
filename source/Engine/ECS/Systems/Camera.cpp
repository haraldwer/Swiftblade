#include "Camera.h"

#include "Transform.h"
#include "game/game.h"

using namespace ECS; 

void SysCamera::Update(EntityID InEntity, Camera& InComponent, double InDelta)
{
    const Transform& t = GetSystem<SysTransform>().Get(InEntity);
    const CameraInstance c {
        t.Position,
       Vec3F::Forward(),
        InComponent.FOV };
    Game::Get().GetRenderScene().SetCamera(c);
}
