#include "RoomSubEditor.h"

#include "Engine/ECS/Entity.h"
#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Game/Systems/CubeVolume.h"

void RoomSubEditor::SetCubeVolume()
{
    // Cache CubeVolume
    const auto& sys = ECS::Manager::Get().GetSystem<ECS::SysCubeVolume>();
    const auto volumes = sys.GetEntities();
    if (!volumes.empty())
        CubeVolume = volumes[0];

    // Force reset cube transform 
    if (CubeVolume != ECS::InvalidID)
        if (ECS::Transform* cubeTrans = ECS::Manager::Get().GetComponent<ECS::Transform>(CubeVolume))
            cubeTrans->SetWorld(Mat4F());
}

ECS::CubeVolume& RoomSubEditor::GetVolume() const
{
    CHECK_ASSERT(CubeVolume == ECS::InvalidID, "Invalid id");
    auto* vol = ECS::Manager::Get().GetComponent<ECS::CubeVolume>(CubeVolume);
    CHECK_ASSERT(!vol, "Invalid ptr")
    return *vol; 
}
