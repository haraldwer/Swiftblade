#pragma once

#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"
#include "Engine/Property/Property.h"
#include "Engine/Property/PropertyOwner.h"
#include "Engine/Rendering/Instances/MeshInstance.h"
#include "Engine/Rendering/Resources/Model.h"

struct VolumeDresser : ECS::Component<VolumeDresser> 
{
};

class SysVolumeDresser : public ECS::System<VolumeDresser>
{
public:
    void Init(ECS::EntityID InID, VolumeDresser& InComponent) override;
    void Dress(ECS::EntityID InID);
    void Frame(ECS::EntityID InID, VolumeDresser& InComponent, double InDelta) override;
    bool Edit(ECS::EntityID InID) override;
    int GetPriority() const override { return -102; }
    bool ShouldUpdate() const override { return true; }

};
