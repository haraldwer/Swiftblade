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
        void Frame(EntityID InEntity, Mesh& InComponent) override;
        bool ShouldUpdate() const override { return true; }
        int GetPriority() const override { return -100; }
    };
}

