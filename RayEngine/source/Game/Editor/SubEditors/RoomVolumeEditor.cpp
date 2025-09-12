#include "RoomVolumeEditor.h"

#include "Blueprints/Blueprint.h"
#include "ECS/Systems/Transform.h"
#include "ECS/Volume/CubeVolume.h"
#include "Editor/RoomEditor.h"
#include "Editor/Room/Room.h"
#include "Engine/ECS/Manager.h"
#include "Engine/Instance/Instance.h"
#include "ImGui/imgui.h"
#include "Utility/History/History.h"

void RoomVolumeEditor::Create()
{
    // Create / Cache CubeVolume
    const auto& ecs = ECS::Manager::Get(); 
    auto& sys = ecs.GetSystem<ECS::SysCubeVolume>();
    const auto volumes = sys.GetEntities();
    if (!volumes.empty())
    {
        cubeVolume = volumes[0];
    }
    else
    {
        if (const auto bp = ResBlueprint("Blueprints/BP_CubeVolume.json").Get())
            cubeVolume = bp->Instantiate();
    }

    // Force reset cube transform 
    if (cubeVolume != ECS::INVALID_ID)
        if (auto* cubeTrans = ecs.GetComponent<ECS::Transform>(cubeVolume))
            cubeTrans->SetWorld(Mat4F());
}

void RoomVolumeEditor::Init()
{
    const auto& ecs = ECS::Manager::Get(); 
    auto& sys = ecs.GetSystem<ECS::SysCubeVolume>();
    
    // Copy volume data to system
    auto& v = sys.Get(cubeVolume); 
    v.data = GetRoom().volumeData;
    v.UpdateCache(Mat4F());
}

void RoomVolumeEditor::Update()
{
    CHECK_RETURN(!IsCurrent());
    CHECK_RETURN(!GetEditor().CanEdit())
    
    auto trace = CameraTrace(8);
    
    if (Input::Action::Get("LM").Down())
    {
        // Update placement :3
        auto& v = GetVolume();
        Vec3F objPos = v.CoordToPos(editEnd);
        editEnd = v.PosToCoord(DragMove(objPos)); 
    }
    else if (Input::Action::Get("LM").Released())
    {
        uint8 placeValue = Input::Action::Get("Ctrl").Down() ? 0 : 1;
        struct VolumeChange
        {
            ECS::CubeVolumeData orgVolume;
            ECS::VolumeCoord start;
            ECS::VolumeCoord end;
            uint8 val;
        };
    
        GetHistory().AddChange(Utility::Change<VolumeChange>(
            [&](const VolumeChange& InData)
            {
                auto& localSystem = ECS::Manager::Get().GetSystem<ECS::SysCubeVolume>();
                localSystem.Set(GetVolumeID(), InData.start, InData.end, InData.val);
                GetRoom().volumeData = GetVolume().data;
            },
            [&](const VolumeChange& InData)
            {
                SetData(InData.orgVolume);
            },
            {
                GetVolume().data,
                editStart,
                editEnd,
                placeValue,
            }));
    }
    else
    {
        // Show the trace location
        editStart = trace;
        editEnd = trace;
    }
    
}

void RoomVolumeEditor::Frame()
{
    CHECK_RETURN(!IsCurrent());
    CHECK_RETURN(!GetEditor().CanEdit())
    CHECK_RETURN(editStart.key == 0 || editEnd.key == 0)
    auto& sys = ECS::Manager::Get().GetSystem<ECS::SysCubeVolume>();
    sys.DrawEditVolume(GetVolumeID(), ECS::VolumeCoord(editStart), editEnd);
}

void RoomVolumeEditor::DebugDraw()
{
    ImGui::Text("Volume editing mode"); 
    const auto size = GetVolume().data.data.size();
    ImGui::Text("Blocks: %i", static_cast<int>(size));
}

void RoomVolumeEditor::Enter()
{
    auto& v =GetVolume(); 
    v.data = GetRoom().volumeData;
    v.UpdateCache(Mat4F());
}

void RoomVolumeEditor::SetData(const ECS::CubeVolumeData &InData) const
{
    GetRoom().volumeData = InData;
    auto& v = GetVolume();
    v.data = InData;
    v.UpdateCache(Mat4F());
}
