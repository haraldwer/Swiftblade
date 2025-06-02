#include "RoomSubEditor.h"

#include "ECS/Volume/CubeVolume.h"
#include "Engine/ECS/Entity.h"
#include "Engine/ECS/Manager.h"
#include "Instance/Instance.h"
#include "Rendering/Scene/Instances/CameraInstance.h"
#include "RoomSubEditorManager.h"

void RoomSubEditor::SetOwner(RoomSubEditorManager* InOwner)
{
    owner = InOwner; 
}

ECS::EntityID RoomSubEditor::GetVolumeID() const
{
    CHECK_ASSERT(!owner, "Invalid owner")
    return owner->GetCubeVolume();
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
    CHECK_ASSERT(!owner, "Invalid owner");
    return owner->GetHistory();
}

RoomType RoomSubEditor::GetType() const
{
    return owner->GetType();
}

Vec3F RoomSubEditor::UpdateCameraTrace()
{
    auto& sys = ECS::Manager::Get().GetSystem<ECS::SysCubeVolume>();
    const CameraInstance cam = Engine::Instance::Get().GetRenderScene().GetCamera();
    const Coord coord = sys.Trace(
        GetVolumeID(),
        cam.position,
        Mat4F(cam.rotation).Forward(),
        7);

    const Vec3F pos = GetVolume().CoordToPos(coord);
    const float dt = static_cast<float>(Utility::Time::Get().Delta());
    lastTracePos = Lerp(lastTracePos, pos, 50.0f * dt);
    return lastTracePos;
}
