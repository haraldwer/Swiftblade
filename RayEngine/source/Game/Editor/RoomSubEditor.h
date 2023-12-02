#pragma once
#include "Engine/ECS/Entity.h"

class RoomSubEditorManager;

namespace Utility
{
    class History;
}

namespace ECS
{
    struct CubeVolume;
    struct Transform;
}

class RoomSubEditor
{
public:
    virtual ~RoomSubEditor() = default;

    virtual void Init() {}
    virtual void Deinit() {}
    virtual void Update(double InDelta) {}
    virtual void UpdateUI(bool InIsCameraControlling) {}
    virtual void Enter() {}
    virtual void Exit() {}
    
    void SetOwner(RoomSubEditorManager* InOwner);
    
    ECS::EntityID GetVolumeID() const;
    ECS::CubeVolume& GetVolume() const;
    Utility::History& GetHistory() const;

private:
    
    RoomSubEditorManager* Owner = nullptr; 
};
