#include "RoomSubEditor.h"

#include "RoomEditor.h"
#include "ECS/Volume/CubeVolume.h"
#include "Engine/ECS/Entity.h"
#include "Engine/ECS/Manager.h"
#include "Instance/Instance.h"
#include "Rendering/Scene/Instances/CameraInstance.h"
#include "SubEditors/RoomVolumeEditor.h"

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

Type RoomSubEditor::GetCurrent() const
{
    CHECK_ASSERT(!editor, "Invalid editor");
    return editor->GetSubEditors().GetCurrent();
}

Vec3F RoomSubEditor::CameraTrace(const int32 InDist) const
{
    auto& sys = ECS::Manager::Get().GetSystem<ECS::SysCubeVolume>();
    const CameraInstance cam = Engine::Instance::Get().GetRenderScene().GetCamera();
    const ECS::VolumeCoord coord = sys.Trace(
        GetVolumeID(),
        cam.position,
        Mat4F(cam.rotation).Forward(),
        InDist);
    return GetVolume().CoordToPos(coord);
}
