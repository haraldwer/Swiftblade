#pragma once

#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"
#include "Engine/Physics/Resources/Material.h"

namespace ECS
{

    
    struct Collider : Component<Collider>
    {
        PROPERTY_C(uint8, Shape, 0);
        PROPERTY_C(Vec4F, ShapeData, Vec4F::One()); // Shape data depends on shape type
        PROPERTY_C(ResPM, Material, "Defaults/PM_Default.json");
    };

    class SysCollider : public System<Collider>
    {
    public:
        
        void Init(EntityID InEntity, Collider& InComponent) override;
        void Deinit(EntityID InEntity, Collider& InComponent) override;
        void Update(EntityID InID, Collider& InComponent, double InDelta) override;
        bool ShouldUpdate() const override;
    };
}
