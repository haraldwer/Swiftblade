#pragma once

#include "Collections/PersistanceContainer.h"
#include "ECS/System.h"
#include "ECS/Component.h"
#include "Rendering/Resources/Model.h"
#include "Rendering/Resources/Material.h"
#include "Scene/Instances/MeshInstance.h"

namespace ECS
{
    struct Mesh : Component<Mesh>
    { 
        PROPERTY_D(bool, Visible, true);
        PROPERTY_D(uint8, Mask, static_cast<uint8>(Rendering::MeshMask::DEFAULT) | static_cast<uint8>(Rendering::MeshMask::SHADOWS));
        PROPERTY_D(ResModel, Model, "Defaults/M_Cube.obj");
        PROPERTY_D(ResRM, Material, "Defaults/RM_Default.json");

        Mat4F worldCache = {};
        uint64 hashCache = 0;
    };

    class SysMesh : public System<Mesh>
    {
    public:
        void SystemInit() override;
        void SystemFrame() override;
        bool ShouldUpdate() const override { return true; }
        int GetPriority() const override { return -100; }

    private:
        
        Utility::PersistanceContainer<uint64> diff;
        Map<uint64, Set<ComponentID>> hashToComponent;
        uint32 persistentID = 0;
    };
}

