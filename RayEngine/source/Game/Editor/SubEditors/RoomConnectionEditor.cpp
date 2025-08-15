#include "RoomConnectionEditor.h"

#include "ECS/Manager.h"
#include "ECS/RoomConnection.h"
#include "ECS/Systems/Transform.h"
#include "ECS/Volume/CubeVolume.h"
#include "Editor/RoomEditor.h"
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
    if (startEntity == ECS::INVALID_ID)
    {
        if (const auto bp = config.StartBP.Get().Get())
        {
            const Vec3F center = v.CoordToPos(centerCoord) + GetOff(true);
            startEntity = bp->Instantiate(center);
            sys.Get<RoomConnection>(startEntity).IsEnd = false; 
        }
    }
    
    ECS::VolumeCoord endCoord = GetRoom().Connection.Get();
    if (endCoord.key == 0)
        endCoord = centerCoord;
    if (endEntity == ECS::INVALID_ID)
        if (const auto bp = config.EndBP.Get().Get())
            endEntity = bp->Instantiate(v.CoordToPos(endCoord) + GetOff(false));
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
    ECS::VolumeCoordKey& coord = GetRoom().Connection.Get();
    if (coord == 0)
        coord = CameraOffset(6).key;
    Vec3F pos = GetVolume().CoordToPos(coord) + GetOff(false);
    trans->SetPosition(Lerp(trans->GetPosition(), pos, config.LerpSpeed.Get()));

    CHECK_RETURN(!IsCurrent());
    CHECK_RETURN(!GetEditor().CanEdit())

    auto addChange = [&](const ECS::VolumeCoordKey InPrev)
    {
        struct RoomConnectionChange
        {
            ECS::VolumeCoord prevCoord;
            ECS::VolumeCoord newCoord;
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
                InPrev,
                coord
            }));
    };
    
    if (Input::Action::Get("Ctrl").Down())
    {
        if (Input::Action::Get("LM").Pressed())
        {
            selectCoord = coord;
            coord = CameraOffset(6).key;
        }
    }
    else
    {
        if (Input::Action::Get("LM").Pressed())
            selectCoord = coord;
        
        if (Input::Action::Get("LM").Down())
        {
            auto& v = GetVolume();
            Vec3F newPos = DragMove(v.CoordToPos(coord));
            coord = v.PosToCoord(newPos).key;
        }
    }
        
    if (Input::Action::Get("LM").Released() && selectCoord.key != 0)
        addChange(selectCoord.key);
}

ECS::EntityID RoomConnectionEditor::GetConnection(bool InSnap) const
{
    if (InSnap)
        if (auto* trans = ECS::Manager::Get().GetComponent<ECS::Transform>(endEntity))
            trans->SetPosition(GetVolume().CoordToPos(GetRoom().Connection.Get()) + GetOff(false));
    return endEntity;
}

Vec3F RoomConnectionEditor::GetOff(bool IsStart) const
{
    return GetVolume().Scale * config.Offset.Get() * (IsStart ? Vec3F::One() : Vec3F(1, 1, -1));
}
