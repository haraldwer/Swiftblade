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
    Utility::History& GetHistory() { return History; }
    ECS::EntityID& GetCubeVolume() { return CubeVolume; }
    
    bool IgnoreSave(ECS::EntityID InID) const; 
    Mat4F GetStartOffset() const { return PathEditor.GetStartOffset(); }
    RoomType GetType() const { return Type; }

private: 
    
    SubEditorMode Mode = SubEditorMode::PATH;
    Utility::History History = {}; // Shared history for all subeditors 
    
    RoomPathEditor PathEditor = {};
    RoomGenEditor GenEditor = {};
    RoomVolumeEditor VolumeEditor = {};
    RoomObjectEditor ObjectEditor = {};
    
    ECS::EntityID CubeVolume = ECS::InvalidID;
    RoomType Type = RoomType::ROOM; 
};
