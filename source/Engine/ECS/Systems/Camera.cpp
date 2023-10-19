#include "Camera.h"

#include "Transform.h"
#include "game/game.h"

using namespace ECS; 

void SysCamera::Init(EntityID InEntity, Camera& InComponent)
{
    // Setup camera
    
    //InComponent.Cam.position = Vector3{ 0.0f, 0.0f, 0.0f };
    //InComponent.Cam.target = Vector3{ 0.0f, 0.0f, 0.0f };
    //InComponent.Cam.up = Vector3{ 0.0f, 1.0f, 0.0f };
    //InComponent.Cam.fovy = 60.0f;
    //InComponent.Cam.projection = CAMERA_PERSPECTIVE;
}

void SysCamera::Update(EntityID InEntity, Camera& InComponent, double InDelta)
{
    //int x = IsKeyDown(KEY_W) - IsKeyDown(KEY_S);
    //int y = IsKeyDown(KEY_Q) - IsKeyDown(KEY_E);
    //int z = IsKeyDown(KEY_A) - IsKeyDown(KEY_D);
    //
    //auto& trans = GetSystem<SysTransform>().Get(InEntity);
    //trans.Trans.translation.x += x * InDelta;
    //trans.Trans.translation.y += y * InDelta;
    //trans.Trans.translation.z += z * InDelta;
    //
    //// Relative to forward
    //
    //InComponent.Cam.position = trans.Trans.translation;
    //
    //Game::Get().GetRenderScene().SetCamera(InComponent.Cam);
}
