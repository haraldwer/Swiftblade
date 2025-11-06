#pragma once
#include "Game/ECS/Volume/CubeVolumeCoord.h"
#include "Engine/ECS/Entity.h"
#include "Level/RoomInfo.h"

struct EditRoom;
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
    virtual void DebugDraw() {}

    virtual void Enter() {}
    virtual void Exit() {}
    virtual bool IgnoreSave(ECS::EntityID InID) { return false; }
    virtual bool IsEnabled(RoomType InRoomType) { return true; }
    
    ECS::EntityID GetVolumeID() const;
    ECS::CubeVolume& GetVolume() const;
    Utility::History& GetHistory() const;
    RoomEditor& GetEditor() const;
    EditRoom& GetRoom() const;

    Type GetCurrent() const;
    bool IsCurrent() const { return GetCurrent() == GetObjType(); }
    
    ECS::VolumeCoord CameraTrace(int32 InDist = 7) const;
    ECS::VolumeCoord CameraOffset(float InDist = 5.0f) const;
    Vec3F DragMove(Vec3F InRef) const;
    Vec3F DragMoveDelta(const Vec3F &InRef) const;
    
private:
    String option;
    RoomEditor* editor = nullptr;
};
