#pragma once

#include "ECS/Component.h"
#include "ECS/System.h"
#include "Physics/Resources/Material.h"

namespace Physics
{
    enum class Shape : uint8;
}

namespace ECS
{
    struct Collider : Component<Collider>
    {
        PROPERTY_D(ResPM, Material, "Defaults/PM_Default.json");

        // 0 - Box: x, y, z = extents
        // 1 - Capsule: x = radius, y = halfheight
        // 2 - Sphere: x = radius
        PROPERTY_D(uint8, Shape, 0);
        PROPERTY_D(Vec4F, ShapeData, Vec4F::One()); // Shape data depends on shape type
        PROPERTY_D(bool, IsTrigger, false); 
    };

    class SysCollider : public System<Collider>
    {
    public:
        
        void Init(EntityID InEntity, Collider& InComponent) override;
        void Deinit(EntityID InEntity, Collider& InComponent) override;
        void Update(EntityID InID, Collider& InComponent) override;
        bool ShouldUpdate() const override;
        int GetPriority() const override { return 99; }

        static void UpdateShape(EntityID InEntity);
    };
}
