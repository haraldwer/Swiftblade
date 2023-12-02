#include "RoomConnectionEditor.h"

#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Game/ECS/RoomConnection.h"
#include "ImGui/imgui.h"
#include "Utility/History/History.h"

void RoomConnectionEditor::Init()
{
    // Get / create connections
    auto& sys = ECS::Manager::Get().GetSystem<SysRoomConnection>();
    for (const ECS::EntityID connection : sys.GetEntities())
    {
        if (sys.Get<RoomConnection>(connection).IsEnd)
            EndEntity = connection;
        else
            StartEntity = connection;
    }
    
    if (EndEntity == ECS::InvalidID)
        if (const auto bp = Config.EndBP.Get().Get())
            EndEntity = bp->Instantiate();

    if (StartEntity == ECS::InvalidID)
    {
        if (const auto bp = Config.EndBP.Get().Get())
        {
            StartEntity = bp->Instantiate();
            sys.Get<RoomConnection>(StartEntity).IsEnd = false; 
        }
    }

    SetMode(EditMode::START);
}

void RoomConnectionEditor::Deinit()
{
    if (StartEntity != ECS::InvalidID)
    {
        ECS::Manager::Get().DestroyEntity(StartEntity);
        StartEntity = ECS::InvalidID;
        LOG("Destroyed start")
    }
}

void RoomConnectionEditor::Update(double InDelta)
{
}

void RoomConnectionEditor::UpdateUI(bool InIsCameraControlling)
{
    ImGui::Text("Connection editing mode"); 
    
    // Draw the gizmo on end
    ImGui::Text(("Mode: " + std::to_string(static_cast<uint8>(Mode))).c_str());

    if (ImGui::Button("START"))
        SetMode(EditMode::START);
    ImGui::SameLine(); 
    if (ImGui::Button("END"))
        SetMode(EditMode::END);

    const ECS::EntityID id = GetCurrent();
    CHECK_RETURN(id == ECS::InvalidID);
    if (ECS::Manager::Get().GetSystem<ECS::SysTransform>().Edit(id))
    {
        // Add to history!
        const ECS::EntityID currID = GetCurrent();
        const struct EditData
        {
            Mat4F PrevTrans;
            Mat4F NewTrans;
            ECS::EntityID ID; 
        } data {
            EditStartTrans,
            GetTrans(currID),
            currID
        };

        GetHistory().AddChange(Utility::Change<EditData>(
            [&](const EditData& InData)
            {
                if (InData.ID != ECS::InvalidID)
                    if (const auto trans = ECS::Manager::Get().GetComponent<ECS::Transform>(InData.ID))
                        trans->SetWorld(InData.NewTrans);
                EditStartTrans = data.NewTrans;
            },
            [&](const EditData& InData)
            {
                if (InData.ID != ECS::InvalidID)
                    if (const auto trans = ECS::Manager::Get().GetComponent<ECS::Transform>(InData.ID))
                        trans->SetWorld(InData.PrevTrans);
                EditStartTrans = data.PrevTrans;
            },
            data));
    }
}

Mat4F RoomConnectionEditor::GetStartOffset() const
{
    return Mat4F::GetInverse(GetTrans(StartEntity));
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

Mat4F RoomConnectionEditor::GetTrans(const ECS::EntityID InID)
{
    if (InID != ECS::InvalidID)
        if (const auto trans = ECS::Manager::Get().GetComponent<ECS::Transform>(InID))
            return trans->World();
    return {}; 
}

void RoomConnectionEditor::SetMode(const EditMode InMode)
{
    Mode = InMode;
    EditStartTrans = GetTrans(GetCurrent()); 
}
