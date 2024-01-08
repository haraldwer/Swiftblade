#pragma once

#include "Engine/Property/Property.h"
#include "Engine/Property/PropertyOwner.h"
#include "Engine/Rendering/Instances/MeshInstance.h"
#include "Engine/Rendering/Resources/Model.h"

namespace ECS
{
    class SysCubeVolume;
}

namespace ECS
{
    struct CubeVolume;
}

/*
struct DressData : PropertyOwner<DressData>
{
    PROPERTY(ResModel, Model);
    PROPERTY(ResRM, Material);
};

struct DressVisuals : PropertyOwner<DressVisuals>
{
    // For each type, there should be a dress piece
    // For now, start with flat wall
    PROPERTY(DressData, FlatWall);
};
*/

struct CubeVolumeDresser : PropertyOwner<CubeVolumeDresser>
{
    void Dress(const ECS::CubeVolume& InVolume, const ECS::SysCubeVolume& InSys);
    void Draw(); 

    //PROPERTY(DressVisuals, VisualData); 
    
private:
    uint8 EvaluateDress(const Array<uint8, 8>& InNeighbors) const;
    
    struct ModelData
    {
        MeshInstance Mesh;
        Vector<Mat4F> Instances;
    };
    Vector<ModelData> Instances;
    
};
