#pragma once

#include "../RoomSubEditor.h"
#include "Engine/Blueprints/Blueprint.h"
#include "Core/Property/Property.h"
#include "Editor/EditorCamera.h"
#include "Editor/Room/Room.h"

typedef OrderedMap<String, ResBlueprint> RoomTypeMap;

struct RoomObjectEditorConfig : BaseConfig<RoomObjectEditorConfig>
{
    PROPERTY(String, CurrentObj);
    PROPERTY_C(RoomTypeMap, ObjectTypes, RoomTypeMap({
        { "Cube", ResBlueprint("Blueprints/BP_PhysCube.json") },
        { "Enemy", ResBlueprint("Gameplay/Enemies/BP_Enemy.json") },
        { "Enemy1", ResBlueprint("Gameplay/Enemies/BP_Enemy.json") },
        { "Enemy2", ResBlueprint("Gameplay/Enemies/BP_Enemy.json") },
        { "Enemy3", ResBlueprint("Gameplay/Enemies/BP_Enemy.json") },
        { "Enemy4", ResBlueprint("Gameplay/Enemies/BP_Enemy.json") },
        { "Enemy5", ResBlueprint("Gameplay/Enemies/BP_Enemy.json") },
        { "Enemy6", ResBlueprint("Gameplay/Enemies/BP_Enemy.json") },
        { "Enemy7", ResBlueprint("Gameplay/Enemies/BP_Enemy.json") },
        { "Enemy8", ResBlueprint("Gameplay/Enemies/BP_Enemy.json") },
        { "Enemy9", ResBlueprint("Gameplay/Enemies/BP_Enemy.json") },
    }));
    
    PROPERTY_C(float, DotThreshold, 0.8f);
    
    String Name() const override { return "RoomObjectEditor"; }
};

// A world-space miniature hover menu
class RoomHoverMenu
{
public:
    void Init(RoomObjectEditorConfig& InConfig);
    void Deinit();
    void Update(const EditorCamera &InCamera);
    String TryPick(const EditorCamera &InCamera, float InDotThreshold);

private:

    Mat4F GetTrans(const EditorCamera &InCamera, int InIndex); 
    
    struct Entry
    {
        String name;
        ECS::EntityID id = ECS::INVALID_ID;
    };
    Vector<Entry> objectTypes;

    float wheelRot = 0.0f;
    Vec2F wheelScale = Vec2F(0.3f, 0.15f);
    float objRot = 0.0f;
    float depth = 0.4f;
    Vec2F screenPos = Vec2F(0, 1);
    Vec3F scale = Vec3F(0.015f);
};

class RoomObjectEditor : public RoomSubEditor
{
    TYPE_INFO(RoomObjectEditor, RoomSubEditor);
    
public:
    void Init() override;
    void Deinit() override;
    void Update() override;
    void Frame() override;
    void Enter() override;
    void Exit() override;

    void PlaceObject();
    void RemoveObject();
    
    // Automatically create object if it doesnt exist
    // Move object smoothly to the coord location and rotation, or snap  
    ECS::EntityID GetObject(const RoomObject& InObj, bool InSnap = false);

private:

    ECS::EntityID LoadObject(const RoomObject &InObj) const;
    void LoadRoom();
    void DestroyLoaded();

    struct LoadedObject
    {
        ECS::EntityID id = ECS::INVALID_ID;
        RoomObject obj;
    };
    Map<ECS::VolumeCoordKey, LoadedObject> loadedObjects;
    LoadedObject placeObject;
    
    RoomObjectEditorConfig config = {};
    RoomHoverMenu hoverMenu;
};
