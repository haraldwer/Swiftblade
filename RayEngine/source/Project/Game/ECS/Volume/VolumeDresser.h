#pragma once

#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"
#include "Engine/Property/Property.h"
#include "Engine/Property/PropertyOwner.h"
#include "Engine/Rendering/Instances/MeshInstance.h"
#include "Engine/Rendering/Resources/Model.h"

struct DressEntry : PropertyOwner<DressEntry>
{
    PROPERTY_D(String, Name, "Unnamed");
    PROPERTY_D(ResModel, Model, "Defaults/M_Cube.obj");
    PROPERTY_D(ResRM, Material, "Dressing/RM_DressWall.json");
    
    typedef Array<uint8, 8> TestArray; 
    PROPERTY(TestArray, Test);
};

struct DressData : PropertyOwner<DressData> 
{
    PROPERTY(Vector<DressEntry>, Data);
};

struct VolumeDresser : ECS::Component<VolumeDresser> 
{
    PROPERTY_C(String, DressPath, "Dressing/DA_Dress_Default.json");

    DressData Data;
    
    struct ModelData
    {
        MeshInstance Mesh;
        Vector<Mat4F> Instances;
        Vec3F MinBounds;
        Vec3F MaxBounds;
    };
    Vector<ModelData> Instances;
    Vector<Vec3F> DebugMissing;
};

class SysVolumeDresser : public ECS::System<VolumeDresser>
{
public:
    void Init(ECS::EntityID InID, VolumeDresser& InComponent) override;
    void Frame(ECS::EntityID InID, VolumeDresser& InComponent, double InDelta) override;
    bool Edit(ECS::EntityID InID) override;
    int GetPriority() const override { return -102; }
    bool ShouldUpdate() const override { return true; }
    
    void Dress(ECS::EntityID InID, bool InRandomize = false);
    
private:
    bool EvaluateCoord(const Vector<DressEntry>& InDressData, const Array<uint8, 8>& InNeighbors, uint8& OutDress, uint8& OutVariation) const;
    
};
