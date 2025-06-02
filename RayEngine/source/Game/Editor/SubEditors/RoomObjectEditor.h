#pragma once

#include "../RoomSubEditor.h"
#include "Engine/Blueprints/Blueprint.h"
#include "Core/Property/Property.h"

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

    ECS::EntityID GetEditEntity() const { return objectID; }

private:
    
    // Edit object
    void SetEditObject(int InIndex);
    void NewEditObject();
    void DestroyEditObject();
    ECS::EntityID objectID = ECS::INVALID_ID;

    // Placed objects
    struct ObjectData
    {
        Vec3F position = {};
        Vec3F rotation = {};
        int objectType = -1;
        ECS::EntityID id = ECS::INVALID_ID;
    };
    void LoadPlacedObjects();
    void PlaceObject(const ObjectData& InObjectData);
    void RemovePlacedObject(uint32 InKey);
    uint32 GetKey(const Vec3F& InPos) const; 
    ObjectData GetPlacedObjectData(uint32 InKey) const;
    Map<uint32, ObjectData> placedObjects = {};

    // Creation
    ECS::EntityID CreateObject(int InIndex, const Mat4F& InMat) const;
    
    RoomObjectEditorConfig config = {}; 
    
    Vec3F targetRot = {};
    Vec3F targetPos = {}; 
};
