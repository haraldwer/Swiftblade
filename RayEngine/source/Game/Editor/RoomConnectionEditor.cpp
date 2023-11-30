#include "RoomConnectionEditor.h"

#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Game/Systems/RoomEnd.h"
#include "ImGui/imgui.h"

void RoomConnectionEditor::Init()
{
    SetCubeVolume();

    // Cache / create end
    const auto& sys = ECS::Manager::Get().GetSystem<SysRoomEnd>();
    const auto roomEnds = sys.GetEntities();
    if (!roomEnds.empty())
        EndEntity = roomEnds[0];
    else if (const auto bp = Config.EndBP.Get().Get())
        EndEntity = bp->Instantiate();
    
    // Create start visualization
    if (StartEntity != ECS::InvalidID)
    {
        ECS::Manager::Get().DestroyEntity(StartEntity);
        StartEntity = ECS::InvalidID; 
    }        
    if (auto bp = Config.EndBP.Get().Get())
        StartEntity = bp->Instantiate();
}

void RoomConnectionEditor::Deinit()
{
    if (StartEntity != ECS::InvalidID)
    {
        ECS::Manager::Get().DestroyEntity(StartEntity);
        StartEntity = ECS::InvalidID;
    }
}

void RoomConnectionEditor::Update(double InDelta)
{
}

void RoomConnectionEditor::UpdateUI()
{
    ImGui::Text("Connection editing mode"); 
    
    // Draw the gizmo on end
    ImGui::Text(("Mode: " + std::to_string(static_cast<uint8>(Mode))).c_str());

    if (ImGui::Button("START"))
        Mode = EditMode::START;
    ImGui::SameLine(); 
    if (ImGui::Button("END"))
        Mode = EditMode::END;

    const ECS::EntityID id = GetCurrent();
    CHECK_RETURN(id == ECS::InvalidID);
    ECS::Manager::Get().GetSystem<ECS::SysTransform>().Edit(id);
}

Mat4F RoomConnectionEditor::GetStartOffset() const
{
    if (const auto trans = ECS::Manager::Get().GetComponent<ECS::Transform>(StartEntity))
        return trans->World();
    return Mat4F(); 
}

ECS::EntityID RoomConnectionEditor::GetCurrent() const
{
    switch (Mode) {
    case EditMode::START:
        return StartEntity;
    case EditMode::END:
        return EndEntity;
    }
    return ECS::InvalidID;
}
