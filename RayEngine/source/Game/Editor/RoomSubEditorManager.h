#pragma once

#include "RoomConnectionEditor.h"
#include "RoomObjectEditor.h"
#include "VolumeEditor.h"

enum class SubEditorMode : uint8
{
    VOLUME,
    OBJECTS,
    CONNECTIONS,
    COUNT
};

class RoomSubEditorManager
{
public:
    
    void Init();
    void Deinit();
    void Update(double InDelta, bool InIsCameraControlling);
    void UpdateUI(bool InIsCameraControlling);
    
    void SetMode(SubEditorMode InMode);
    RoomSubEditor& GetSubEditor(SubEditorMode InMode);
    Utility::History& GetHistory() { return History; }
    ECS::EntityID& GetCubeVolume() { return CubeVolume; }
    
    bool IgnoreSave(ECS::EntityID InID) const; 
    Mat4F GetStartOffset() const { return ConnectionEditor.GetStartOffset(); }

private: 
    
    SubEditorMode Mode = SubEditorMode::VOLUME;
    Utility::History History; // Shared history for all subeditors 
    
    VolumeEditor VolumeEditor;
    RoomObjectEditor ObjectEditor;
    RoomConnectionEditor ConnectionEditor;
    
    ECS::EntityID CubeVolume = ECS::InvalidID; 
};
