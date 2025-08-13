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
    virtual void Frame() {}
    virtual void DebugDraw() {}
    virtual void Enter() {}
    virtual void Exit() {}
    virtual bool IgnoreSave(ECS::EntityID InID) { return false; }
    
    void SetOwner(RoomSubEditorManager* InOwner);
    
    ECS::EntityID GetVolumeID() const;
    ECS::CubeVolume& GetVolume() const;
    Utility::History& GetHistory() const;

    RoomType GetType() const;
    Vec3F UpdateCameraTrace();

protected:
    RoomSubEditorManager* owner = nullptr;
    
private:
    
    Vec3F lastTracePos = {};
    
};
