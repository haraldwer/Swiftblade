#include "RoomSubEditor.h"

#include "ECS/Volume/CubeVolume.h"
#include "Engine/ECS/Entity.h"
#include "Engine/ECS/Manager.h"
#include "Instance/Instance.h"
#include "Rendering/Instances/CameraInstance.h"
#include "RoomSubEditorManager.h"

void RoomSubEditor::SetOwner(RoomSubEditorManager* InOwner)
{
    Owner = InOwner; 
}

ECS::EntityID RoomSubEditor::GetVolumeID() const
{
    CHECK_ASSERT(!Owner, "Invalid owner")
    return Owner->GetCubeVolume();
}

ECS::CubeVolume& RoomSubEditor::GetVolume() const
{
    const ECS::EntityID cube = GetVolumeID(); 
    CHECK_ASSERT(cube == ECS::InvalidID, "Invalid id");
    auto* vol = ECS::Manager::Get().GetComponent<ECS::CubeVolume>(cube);
    CHECK_ASSERT(!vol, "Invalid ptr")
    return *vol; 
}

Utility::History& RoomSubEditor::GetHistory() const
{
    CHECK_ASSERT(!Owner, "Invalid owner");
    return Owner->GetHistory();
}

RoomType RoomSubEditor::GetType() const
{
    return Owner->GetType();
}

Vec3F RoomSubEditor::UpdateCameraTrace()
{
    auto& sys = ECS::Manager::Get().GetSystem<ECS::SysCubeVolume>();
    const CameraInstance cam = Engine::Instance::Get().GetRenderScene().GetCamera();
    Coord coord = sys.Trace(
        GetVolumeID(),
        cam.Position,
        Mat4F(cam.Rotation).Forward(),
        7);

    Vec3F pos = GetVolume().CoordToPos(coord);
    float dt = static_cast<float>(Utility::Time::Get().Delta());
    LastTracePos = Lerp(LastTracePos, pos, 50.0f * dt);
    return LastTracePos;
}
