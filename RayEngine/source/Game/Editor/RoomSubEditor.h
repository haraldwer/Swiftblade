#pragma once
#include "Engine/ECS/Entity.h"

class RoomEditor;

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
    BASE_TYPE_INFO(RoomSubEditor);
    friend class RoomSubEditorManager;
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
    
    ECS::EntityID GetVolumeID() const;
    ECS::CubeVolume& GetVolume() const;
    Utility::History& GetHistory() const;
    RoomEditor& GetEditor() const;

    Type GetCurrent() const;
    bool IsCurrent() const { return GetCurrent() == GetObjType(); }
    Vec3F CameraTrace(int32 InDist) const;
    
private:
    RoomEditor* editor = nullptr;
    
};
