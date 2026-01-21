#include "Light.h"

#include "Instance/Instance.h"
#include "Transform.h"

void ECS::SysLight::Frame(const EntityID InID, Light& InComponent)
{
    if (!InComponent.Visible)
        return;

    while (InComponent.id == 0)
    {
        // Generate new hash
        const double t = static_cast<double>(InID) + Utility::Time::Get().Total();
        InComponent.id = Utility::Hash(t);  
    }

    const Transform& t = Get<Transform>(InID);
    auto& s = Engine::Instance::Get().GetRenderScene();
    s.AddLight({
        .data= {
            .position= t.GetPosition(),
            .direction= t.World().Forward(),
            .color= InComponent.Color,
            .range= InComponent.Range,
            .radius= InComponent.Radius,
            .intensity= InComponent.Intensity
        },
        .shadows= InComponent.Shadows,
        .id= InComponent.id
    });
}
