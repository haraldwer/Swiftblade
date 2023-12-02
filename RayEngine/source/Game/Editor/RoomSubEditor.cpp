#include "RoomSubEditor.h"

#include "RoomSubEditorManager.h"
#include "Engine/ECS/Entity.h"
#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Game/Systems/CubeVolume.h"

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
