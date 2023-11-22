#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"
#include "Engine/Rendering/Resources/Model.h"
#include "Engine/Rendering/Resources/Material.h"

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
        void Update(EntityID InEntity, Mesh& InComponent, double InDelta) override;
        bool ShouldUpdate() const override { return true; }
        int GetPriority() const override { return -100; }
    };
}

