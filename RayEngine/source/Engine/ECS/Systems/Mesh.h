#pragma once

#include "ECS/System.h"
#include "ECS/Component.h"
#include "Rendering/Resources/Model.h"
#include "Rendering/Resources/Material.h"

namespace ECS
{
    struct Mesh : Component<Mesh>
    { 
        PROPERTY_D(bool, Visible, true);
        PROPERTY_D(ResModel, Model, "Defaults/M_Cube.obj");
        PROPERTY_D(ResRM, Material, "Defaults/RM_Default.json");
    };

    class SysMesh : public System<Mesh>
    {
        
    public:
        void SystemFrame() override;
        void Update(EntityID InEntity, Mesh& InComponent) override;
        bool ShouldUpdate() const override { return true; }
        int GetPriority() const override { return -100; }
    };
}

