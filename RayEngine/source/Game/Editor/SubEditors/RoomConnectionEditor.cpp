#include "RoomConnectionEditor.h"

#include "ECS/Manager.h"
#include "ECS/RoomConnection.h"
#include "ECS/Systems/Transform.h"
#include "ECS/Volume/CubeVolume.h"
#include "Editor/Room/Room.h"
#include "History/History.h"
#include "Input/Action.h"

void RoomConnectionEditor::Init()
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

    auto& v = GetVolume();
    auto centerCoord = v.GetCenter();
    const Vec3F center = v.CoordToPos(centerCoord);
    if (startEntity == ECS::INVALID_ID)
    {
        if (const auto bp = config.StartBP.Get().Get())
        {
            startEntity = bp->Instantiate(center);
            sys.Get<RoomConnection>(startEntity).IsEnd = false; 
        }
    }

    ECS::VolumeCoord endCoord = GetRoom().Connection.Get();
    if (endCoord.key == 0)
        endCoord = centerCoord;
    if (endEntity == ECS::INVALID_ID)
        if (const auto bp = config.EndBP.Get().Get())
            endEntity = bp->Instantiate(v.CoordToPos(endCoord));
}

void RoomConnectionEditor::Deinit()
{
    ECS::Manager::Get().DestroyEntity(endEntity);
    ECS::Manager::Get().DestroyEntity(startEntity);
    startEntity = ECS::INVALID_ID;
    endEntity = ECS::INVALID_ID;
}

void RoomConnectionEditor::Update()
{
    auto* trans = ECS::Manager::Get().GetComponent<ECS::Transform>(endEntity);
    CHECK_RETURN(!trans);
    
    // Lerp connection towards coord
    ECS::VolumeCoordKey coord = GetRoom().Connection.Get();
    Vec3F pos = GetVolume().CoordToPos(coord);
    const float dt = static_cast<float>(Utility::Time::Get().Delta());
    trans->SetPosition(LerpDelta(trans->GetPosition(), pos, 50.0f, dt));

    CHECK_RETURN(!IsCurrent());

    ECS::VolumeCoord newCoord = CameraOffset();

        
    // TODO: Show some kind of preview...
    // TODO: Drag to move
    
        
    if (Input::Action::Get("LM").Pressed())
    {
        struct RoomConnectionChange
        {
            ECS::VolumeCoord prevCoord;
            ECS::VolumeCoord newCoord;;
        };
        
        GetHistory().AddChange(Utility::Change<RoomConnectionChange>(
            [&](const RoomConnectionChange& InData)
            {
                GetRoom().Connection = InData.newCoord.key;
                    
            },
            [&](const RoomConnectionChange& InData)
            {
                GetRoom().Connection = InData.prevCoord.key;
                    
            },
            {
                ECS::VolumeCoord(GetRoom().Connection.Get()),
                newCoord
            }));
    }
}

ECS::EntityID RoomConnectionEditor::GetConnection(bool InSnap) const
{
    if (InSnap)
        if (auto* trans = ECS::Manager::Get().GetComponent<ECS::Transform>(endEntity))
            trans->SetPosition(GetVolume().CoordToPos(GetRoom().Connection.Get()));
    return endEntity;
}
