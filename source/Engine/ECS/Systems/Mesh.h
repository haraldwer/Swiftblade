#pragma once

#include "../System.h"

namespace ECS
{
    struct Mesh
    {
        bool Visible = true;
        // Point to a mesh and material resource
    };

    class SysMesh : public System<Mesh>
    {
        
    public:

        void Init(EntityID InEntity, Mesh& InComponent) override;
        void Update(EntityID InEntity, Mesh& InComponent, double InDelta) override;
        
    };
}

