#include "Camera.h"

#include "Transform.h"
#include "Editor/Debug/Draw.h"
#include "Instance/Instance.h"

void ECS::SysCamera::Frame(EntityID InEntity, Camera& InComponent)
{
    //Set(InEntity, InComponent);
}

void ECS::SysCamera::Update(const EntityID InEntity, Camera& InComponent)
{
    Set(InEntity, InComponent);
}

void ECS::SysCamera::Set(const EntityID InEntity, Camera& InComponent)
{
    const Transform& t = Get<Transform>(InEntity);
    const Rendering::CameraInstance c {
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
        Debug::Sphere(t.GetPosition());
}
