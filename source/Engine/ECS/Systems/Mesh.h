#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"
#include "Engine/Resource/Resources/ResModel.h"

namespace ECS
{
    struct Mesh : Component
    { 
        PROPERTY_P(bool, Visible, true);
        PROPERTY(ResModel, Model);
    };

    class SysMesh : public System<Mesh>
    {
        
    public:

        void Init(EntityID InEntity, Mesh& InComponent) override;
        void Update(EntityID InEntity, Mesh& InComponent, double InDelta) override;
        
    };
}

