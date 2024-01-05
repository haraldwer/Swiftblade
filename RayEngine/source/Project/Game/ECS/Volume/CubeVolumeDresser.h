#pragma once

#include "Engine/Property/Property.h"
#include "Engine/Property/PropertyOwner.h"
#include "Engine/Rendering/Instances/MeshInstance.h"
#include "Engine/Rendering/Resources/Model.h"

namespace ECS
{
    struct CubeVolume;
}

struct DressData : PropertyOwner<DressData>
{
    // For each type, there should be a dress piece
    // For now, start with flat wall

    PROPERTY(ResModel, FlatWall);
};

struct CubeVolumeDresser : PropertyOwner<CubeVolumeDresser>
{
    void Dress(const ECS::CubeVolume& InVolume);
    void Draw(); 

    PROPERTY(DressData, VisualData); 
    
private:
    
    struct ModelData
    {
        MeshInstance Mesh;
        Vector<Mat4F> Instances;
    };
    Vector<ModelData> Instances;
    
};
