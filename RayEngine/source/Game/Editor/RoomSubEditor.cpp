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

void RoomSubEditor::DebugDraw()
{
    if (IsCurrent())
        ImGui::Text("Current room: %s", this->GetObjName().c_str());
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

Room & RoomSubEditor::GetRoom() const
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
    auto& sys = ECS::Manager::Get().GetSystem<ECS::SysCubeVolume>();
    const CameraInstance cam = Engine::Instance::Get().GetRenderScene().GetCamera();
    return sys.Trace(
        GetVolumeID(),
        cam.position,
        Mat4F(cam.rotation).Forward(),
        InDist);
}

ECS::VolumeCoord RoomSubEditor::CameraOffset(const float InDist) const
{
    const CameraInstance cam = Engine::Instance::Get().GetRenderScene().GetCamera();
    Vec3F targetPos = cam.position + cam.rotation.ForwardDirection() * InDist;
    return GetVolume().PosToCoord(targetPos);
}
