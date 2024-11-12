#pragma once

#include "../RoomSubEditor.h"
#include "Engine/BaseConfig.h"
#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Property/Property.h"

struct RoomObjectEditorConfig : BaseConfig<RoomObjectEditorConfig>
{
    // Currently selected bp
    PROPERTY(int, BPIndex);
    // Available bps 
    PROPERTY_C(Vector<ResBlueprint>, Blueprints, Vector<ResBlueprint>({
        ResBlueprint("Blueprints/BP_PhysCube.json"),
        ResBlueprint("Gameplay/Enemies/BP_Enemy.json")
    })); 
    String Name() const override { return "RoomObjectEditor"; }
};

class RoomObjectEditor : public RoomSubEditor
{
public:
    void Init() override;
    void Deinit() override;
    void Update() override;
    void Frame(bool InIsCameraControlling) override;
    void DebugDraw(bool InIsCameraControlling) override;
    void Enter() override;
    void Exit() override;

    void PlaceObject();
    void RemoveObject();

    ECS::EntityID GetEditEntity() const { return ObjectID; }

private:
    
    // Edit object
    void SetEditObject(int InIndex);
    void NewEditObject();
    void DestroyEditObject();
    ECS::EntityID ObjectID = ECS::InvalidID;

    // Placed objects
    struct ObjectData
    {
        Vec3F Position;
        Vec3F Rotation;
        int ObjectType = -1;
        ECS::EntityID ID = ECS::InvalidID;
    };
    void LoadPlacedObjects();
    void PlaceObject(const ObjectData& InObjectData);
    void RemovePlacedObject(uint32 InKey);
    uint32 GetKey(const Vec3F& InPos) const; 
    ObjectData GetPlacedObjectData(uint32 InKey) const;
    Map<uint32, ObjectData> PlacedObjects;

    // Creation
    ECS::EntityID CreateObject(int InIndex, const Mat4F& InMat) const;
    
    RoomObjectEditorConfig Config; 
    
    Vec3F TargetRot;
    Vec3F TargetPos; 
};
