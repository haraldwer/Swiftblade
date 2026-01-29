#pragma once

#include "ECS/System.h"
#include "ECS/Component.h"
#include "Rendering/Resources/Material.h"

namespace ECS
{
    struct Environment : Component<Environment>
    { 
        PROPERTY_D(bool, Visible, true);
        PROPERTY_D(ResRM, Skybox, "Dressing/Skybox/RM_Skybox.json");
        PROPERTY_D(Vec3F, Shape, Vec3F::Zero()); // Shape data depends on shape type
        // Bounds? 
    };

    class SysEnvironment : public System<Environment>
    {
    public:
        void Frame(EntityID InID, Environment& InComponent) override;
        bool ShouldTick() const override { return true; }
        int GetPriority() const override { return -100; }
    };
}

