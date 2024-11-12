#pragma once
#include "Engine/ECS/Entity.h"
#include "Game/ECS/Volume/CubeVolume.h"

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

enum class RoomType : uint8
{
    ROOM,
    ARENA
};

class RoomSubEditor
{
public:
    virtual ~RoomSubEditor() = default;

    virtual void Init() {}
    virtual void Deinit() {}
    virtual void Update() {}
    virtual void Frame(bool InIsCameraControlling) {}
    virtual void DebugDraw(bool InIsCameraControlling) {}
    virtual void Enter() {}
    virtual void Exit() {}
    
    void SetOwner(RoomSubEditorManager* InOwner);
    
    ECS::EntityID GetVolumeID() const;
    ECS::CubeVolume& GetVolume() const;
    Utility::History& GetHistory() const;

    RoomType GetType() const;
    Vec3F UpdateCameraTrace();

protected:
    RoomSubEditorManager* Owner = nullptr;
    
private:
    
    Vec3F LastTracePos;
    
};
