#include "CameraComponent.h"

#include "Transform.h"
#include "Editor/Debug/Draw.h"
#include "Instance/Instance.h"

using namespace ECS;

void SysCameraComponent::Frame(EntityID InEntity, CameraComponent& InComponent)
{
    //Set(InEntity, InComponent);
}

void SysCameraComponent::Update(const EntityID InEntity, CameraComponent& InComponent)
{
    Set(InEntity, InComponent);
}

void SysCameraComponent::Set(const EntityID InEntity, CameraComponent& InComponent)
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
