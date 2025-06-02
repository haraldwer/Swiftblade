#pragma once

#include "SubEditors/RoomGenEditor.h"
#include "SubEditors/RoomObjectEditor.h"
#include "SubEditors/RoomPathEditor.h"
#include "SubEditors/RoomVolumeEditor.h"
#include "Utility/History/History.h"

enum class SubEditorMode : uint8
{
    PATH,
    GEN,
    VOLUME,
    OBJECTS,
    COUNT
};

class RoomSubEditorManager
{
public:
    
    void Init(RoomType InType);
    void Deinit();
    void Update(bool InIsCameraControlling);
    void Frame(bool InIsCameraControlling);
    void DebugDraw(bool InIsCameraControlling);
    
    void SetMode(SubEditorMode InMode);
    RoomSubEditor& GetSubEditor(SubEditorMode InMode);
    Utility::History& GetHistory() { return history; }
    ECS::EntityID& GetCubeVolume() { return cubeVolume; }
    
    bool IgnoreSave(ECS::EntityID InID) const; 
    Mat4F GetStartOffset() const { return pathEditor.GetStartOffset(); }
    RoomType GetType() const { return type; }

private: 
    
    SubEditorMode mode = SubEditorMode::PATH;
    Utility::History history = {}; // Shared history for all subeditors 
    
    RoomPathEditor pathEditor = {};
    RoomGenEditor genEditor = {};
    RoomVolumeEditor volumeEditor = {};
    RoomObjectEditor objectEditor = {};
    
    ECS::EntityID cubeVolume = ECS::INVALID_ID;
    RoomType type = RoomType::ROOM; 
};
