#pragma once
#include "Game/ECS/Volume/CubeVolumeCoord.h"
#include "Engine/ECS/Entity.h"

struct Room;
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
    BASE_CLASS_INFO(RoomSubEditor);
    friend class RoomSubEditorManager;
public:
    virtual ~RoomSubEditor() = default;

    virtual void Init() {}
    virtual void Deinit();

    virtual void Update() {}
    virtual void Frame() {}
    virtual void DebugDraw();

    virtual void Enter() {}
    virtual void Exit() {}
    virtual bool IgnoreSave(ECS::EntityID InID) { return false; }
    
    ECS::EntityID GetVolumeID() const;
    ECS::CubeVolume& GetVolume() const;
    Utility::History& GetHistory() const;
    RoomEditor& GetEditor() const;
    Room& GetRoom() const;

    Type GetCurrent() const;
    bool IsCurrent() const { return GetCurrent() == GetObjType(); }
    
    ECS::VolumeCoord CameraTrace(int32 InDist = 7) const;
    ECS::VolumeCoord CameraOffset(float InDist = 5.0f) const;
    Vec3F DragMove(Vec3F InRef) const;
    Vec3F DragMoveDelta(const Vec3F &InRef) const;
    
private:
    RoomEditor* editor = nullptr;
    
};
