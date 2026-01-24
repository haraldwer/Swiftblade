#include "RoomSubEditor.h"

#include "RoomEditor.h"
#include "ECS/Volume/CubeVolume.h"
#include "Engine/ECS/Entity.h"
#include "Engine/ECS/Manager.h"
#include "Instance/Instance.h"
#include "Rendering/Scene/Instances/CameraInstance.h"
#include "SubEditors/RoomVolumeEditor.h"

void RoomSubEditor::Deinit()
{
    editor = nullptr;
}

ECS::EntityID RoomSubEditor::GetVolumeID() const
{
    CHECK_ASSERT(!editor, "Invalid editor")
    return editor->GetSubEditors().Get<RoomVolumeEditor>().GetCubeVolumeID();
}

ECS::CubeVolume& RoomSubEditor::GetVolume() const
{
    const ECS::EntityID cube = GetVolumeID(); 
    CHECK_ASSERT(cube == ECS::INVALID_ID, "Invalid id");
    auto* vol = ECS::Manager::Get().GetComponent<ECS::CubeVolume>(cube);
    CHECK_ASSERT(!vol, "Invalid ptr")
    return *vol; 
}

Utility::History& RoomSubEditor::GetHistory() const
{
    CHECK_ASSERT(!editor, "Invalid editor");
    return editor->GetHistory();
}

RoomEditor& RoomSubEditor::GetEditor() const
{
    CHECK_ASSERT(!editor, "Invalid editor");
    return *editor;
}

EditRoom & RoomSubEditor::GetRoom() const
{
    return GetEditor().GetRoom();
}

Type RoomSubEditor::GetCurrent() const
{
    CHECK_ASSERT(!editor, "Invalid editor");
    return editor->GetSubEditors().GetCurrent();
}

ECS::VolumeCoord RoomSubEditor::CameraTrace(const int32 InDist) const
{
    const auto& cam = GetEditor().GetFreeCamera();
    const Vec3F camPos = cam.GetPosition();
    const Vec3F mouseDir = cam.GetMouseDirection();
    auto& sys = ECS::Manager::Get().GetSystem<ECS::SysCubeVolume>();
    ECS::VolumeCoord hit = false;
    return sys.Trace(
        GetVolumeID(),
        camPos,
        mouseDir,
        InDist,
        hit);
}

ECS::VolumeCoord RoomSubEditor::CameraOffset(const float InDist) const
{
    const Rendering::CameraInstance cam = Engine::Instance::Get().GetRenderScene().GetCamera();
    Vec3F targetPos = cam.position + cam.rotation.ForwardDirection() * InDist;
    return GetVolume().PosToCoord(targetPos);
}

Vec3F RoomSubEditor::DragMove(Vec3F InRef) const
{
    // First try to trace
    const auto& cam = GetEditor().GetFreeCamera();
    const Vec3F camPos = cam.GetPosition();
    const Vec3F mouseDir = cam.GetMouseDirection();
    auto& sys = ECS::Manager::Get().GetSystem<ECS::SysCubeVolume>();
    ECS::VolumeCoord hit = false;
    ECS::VolumeCoord trace = sys.Trace(
        GetVolumeID(),
        camPos,
        mouseDir,
        8,
        hit);
    if (hit.key != 0) return GetVolume().CoordToPos(trace);

    // Otherwise just drag on object plane!
    const float objDist = (InRef - camPos).Length();
    return camPos + mouseDir * objDist;
}

Vec3F RoomSubEditor::DragMoveDelta(const Vec3F &InRef) const
{
    return DragMove(InRef) - InRef;
}
