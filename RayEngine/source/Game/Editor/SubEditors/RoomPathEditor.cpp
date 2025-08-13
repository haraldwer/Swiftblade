#include "RoomPathEditor.h"

#include "ECS/RoomConnection.h"
#include "ECS/Volume/CubeVolume.h"
#include "Editor/RoomSubEditorManager.h"
#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"
#include "History/History.h"
#include "ImGui/imgui.h"
#include "Input/Action.h"

void RoomPathEditor::Init()
{
    // Get / create connections
    auto& sys = ECS::Manager::Get().GetSystem<SysRoomConnection>();
    for (const ECS::EntityID connection : sys.GetEntities())
    {
        if (sys.Get<RoomConnection>(connection).IsEnd)
            endEntity = connection;
        else
            startEntity = connection;
    }

    const Vec3F center = GetVolume().GetCenter(true) + Vec3F(1.0, -1.0, 1.0);
    
    if (startEntity == ECS::INVALID_ID)
    {
        if (const auto bp = config.StartBP.Get().Get())
        {
            startEntity = bp->Instantiate(center);
            sys.Get<RoomConnection>(startEntity).IsEnd = false; 
        }
    }
    
    if (endEntity == ECS::INVALID_ID)
        if (const auto bp = config.EndBP.Get().Get())
            endEntity = bp->Instantiate(center);
}

void RoomPathEditor::Deinit()
{
    if (startEntity != ECS::INVALID_ID)
    {
        ECS::Manager::Get().DestroyEntity(startEntity);
        startEntity = ECS::INVALID_ID;
    }
}

void RoomPathEditor::Update()
{
    PROFILE();
    
    // Trace location
    // Draw location
    // If clicked - Set the end transform, do stuff

    CHECK_RETURN(endEntity == ECS::INVALID_ID)

    auto* trans = ECS::Manager::Get().GetComponent<ECS::Transform>(endEntity);
    CHECK_RETURN(!trans);

    const float dt = static_cast<float>(Utility::Time::Get().Delta());
    
    // Move object using trace 
    targetPos = CameraTrace(3) + Vec3F(1.0, -1.0, 1.0);
    if (targetPos != Vec3F::Zero())
    {
        const Vec3F currPos = trans->GetPosition();
        trans->SetPosition(Lerp(currPos, targetPos, 50.0f * dt));
    } 

    if (Input::Action::Get("LM").Pressed())
    {
        struct RoomPathChange
        {
            Vec3F newPos;
            Vec3F prevPos;
        };
        
        GetHistory().AddChange(Utility::Change<RoomPathChange>(
            [&](const RoomPathChange& InData)
            {
                if (const auto t = ECS::Manager::Get().GetComponent<ECS::Transform>(endEntity))
                    t->SetPosition(InData.newPos);
            },
            [&](const RoomPathChange& InData)
            {
                if (const auto t = ECS::Manager::Get().GetComponent<ECS::Transform>(endEntity))
                    t->SetPosition(InData.prevPos);
            },
            {
                targetPos,
                trans->GetPosition()
            }));
    }
}

void RoomPathEditor::Frame()
{
}

void RoomPathEditor::DebugDraw()
{
    ImGui::Text("Connection editing mode"); 
}

Mat4F RoomPathEditor::GetTrans(const ECS::EntityID InID)
{
    if (InID != ECS::INVALID_ID)
        if (const auto trans = ECS::Manager::Get().GetComponent<ECS::Transform>(InID))
            return trans->World();
    return {}; 
}
