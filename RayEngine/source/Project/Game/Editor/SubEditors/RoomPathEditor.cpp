#include "RoomPathEditor.h"

#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Game/ECS/RoomConnection.h"
#include "Game/Editor/RoomSubEditorManager.h"
#include "ImGui/imgui.h"
#include "Utility/History/History.h"

void RoomPathEditor::Init()
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
    
    Vec3F center = GetVolume().GetCenter(true) + Vec3F(1.0, -1.0, 1.0);
    
    if (StartEntity == ECS::InvalidID)
    {
        if (const auto bp = Config.StartBP.Get().Get())
        {
            StartEntity = bp->Instantiate(center);
            sys.Get<RoomConnection>(StartEntity).IsEnd = false; 
        }
    }
    
    if (EndEntity == ECS::InvalidID)
        if (const auto bp = Config.EndBP.Get().Get())
            EndEntity = bp->Instantiate(center);
}

void RoomPathEditor::Deinit()
{
    if (StartEntity != ECS::InvalidID)
    {
        ECS::Manager::Get().DestroyEntity(StartEntity);
        StartEntity = ECS::InvalidID;
    }
}

void RoomPathEditor::Update()
{
    // Trace location
    // Draw location
    // If clicked - Set the end transform, do stuff

    CHECK_RETURN(EndEntity == ECS::InvalidID)

    auto* trans = ECS::Manager::Get().GetComponent<ECS::Transform>(EndEntity);
    CHECK_RETURN(!trans);
    CHECK_RETURN(!Owner);

    float dt = static_cast<float>(Utility::Time::Get().Delta());
    
    // Move object using trace 
    TargetPos = UpdateCameraTrace() + Vec3F(1.0, -1.0, 1.0);
    if (TargetPos != Vec3F::Zero())
    {
        const Vec3F currPos = trans->GetPosition();
        trans->SetPosition(Lerp(currPos, TargetPos, 50.0f * dt));
    } 

    if (Input::Action::Get("LM").Pressed())
    {
        struct RoomPathChange
        {
            Vec3F NewPos;
            Vec3F PrevPos;
        };
        
        GetHistory().AddChange(Utility::Change<RoomPathChange>(
                [&](const RoomPathChange& InData)
                {
                    if (auto t = ECS::Manager::Get().GetComponent<ECS::Transform>(EndEntity))
                        t->SetPosition(InData.NewPos);
                    if (Owner)
                        Owner->SetMode(SubEditorMode::GEN);
                },
                [&](const RoomPathChange& InData)
                {
                    if (auto t = ECS::Manager::Get().GetComponent<ECS::Transform>(EndEntity))
                        t->SetPosition(InData.PrevPos);
                    if (Owner)
                        Owner->SetMode(SubEditorMode::GEN);
                },
                {
                    TargetPos,
                    trans->GetPosition()
                }));
    }
}

void RoomPathEditor::Frame(bool InIsCameraControlling)
{
}

void RoomPathEditor::DebugDraw(bool InIsCameraControlling)
{
    ImGui::Text("Connection editing mode"); 
}

Mat4F RoomPathEditor::GetStartOffset() const
{
    return Mat4F::GetFastInverse(GetTrans(StartEntity));
}

ECS::EntityID RoomPathEditor::GetCurrent() const
{
    return EndEntity;
}

Mat4F RoomPathEditor::GetTrans(const ECS::EntityID InID)
{
    if (InID != ECS::InvalidID)
        if (const auto trans = ECS::Manager::Get().GetComponent<ECS::Transform>(InID))
            return trans->World();
    return {}; 
}
