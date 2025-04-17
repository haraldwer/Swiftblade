#include "C:/Dev/Swiftblade/RayEngine/build/source/Engine/CMakeFiles/Engine.dir/Debug/cmake_pch.hxx"
#include "Environment.h"

#include "Transform.h"
#include "Instance/Instance.h"

void ECS::SysEnvironment::Frame(EntityID InID, Environment& InComponent)
{
    // TODO: Static component optimization
    
    if (!InComponent.Visible)
        return;

    const Transform& t = Get<Transform>(InID);
    auto& s = Engine::Instance::Get().GetRenderScene();
    s.AddEnvironment({
        InComponent.Skybox,
        {},
        InComponent.Shape,
        t.GetPosition()
    });
}
