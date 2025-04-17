#include "Light.h"

#include "Instance/Instance.h"
#include "Transform.h"

void ECS::SysLight::Frame(EntityID InID, Light& InComponent)
{
    if (!InComponent.Visible)
        return;

    while (InComponent.ID == 0)
    {
        // Generate new hash
        const double t = static_cast<double>(InID) + Utility::Time::Get().Total();
        InComponent.ID = Utility::Hash(t);  
    }

    const Transform& t = Get<Transform>(InID);
    auto& s = Engine::Instance::Get().GetRenderScene();
    s.AddLight({
        .Data= {
            .Position= t.GetPosition(),
            .Direction= t.World().Forward(),
            .Color= InComponent.Color,
            .Range= InComponent.Range,
            .Radius= InComponent.Radius,
            .Intensity= InComponent.Intensity
        },
        .Shadows= InComponent.Shadows,
        .ID= InComponent.ID
    });
}
