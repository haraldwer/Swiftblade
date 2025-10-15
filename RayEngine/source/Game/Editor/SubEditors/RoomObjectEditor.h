﻿#pragma once

#include "../EditRoom.h"
#include "../RoomSubEditor.h"
#include "Engine/Blueprints/Blueprint.h"

namespace ECS
{
    class SysTransform;
}

class MenuRoomObjects;
typedef OrderedMap<String, ResBlueprint> RoomTypeMap;

struct RoomObjectEditorConfig : BaseConfig<RoomObjectEditorConfig>
{
    PROPERTY(String, CurrentObj);
    PROPERTY(RoomTypeMap, ObjectTypes);
    
    PROPERTY_C(float, DotThreshold, 0.8f);
    PROPERTY_C(float, LerpSpeed, 0.1f);
    
    String Name() const override { return "RoomObjectEditor"; }
};

class RoomObjectEditor : public RoomSubEditor
{
    CLASS_INFO(RoomObjectEditor, RoomSubEditor);
    
public:
    void Init() override;
    void Deinit() override;
    
    void Update() override;
    void Frame() override;
    void Enter() override;
    void Exit() override;
    
    // Automatically create object if it doesnt exist
    // Move object smoothly to the coord location and rotation, or snap  
    ECS::EntityID LoadObject(const EditRoomObject& InObj);

private:

    void TryPickObject();
    void MovePlaceObject();
    void PlaceObject();
    void RemoveObject();
    
    void UpdateTransforms();
    void UpdateTransform(ECS::SysTransform& InSys, ECS::EntityID InID, const EditRoomObject& InObj);
    Mat4F GetTrans(const EditRoomObject& InObj) const;

    ECS::EntityID CreateObject(const EditRoomObject &InObj) const;
    void LoadRoom();
    void DestroyLoaded();
    void RemoveLoaded(ECS::VolumeCoordKey InKey);

    Map<ECS::VolumeCoordKey, ECS::EntityID> loadedObjects;
    ECS::EntityID placeID = ECS::INVALID_ID;
    EditRoomObject placeObj;
    ECS::VolumeCoord movedFrom;
    
    RoomObjectEditorConfig config = {};
    MenuRoomObjects* menu = nullptr;
};
