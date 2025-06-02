#include "RoomVolumeEditor.h"

#include "ECS/Volume/CubeVolume.h"
#include "Engine/ECS/Manager.h"
#include "Engine/Instance/Instance.h"
#include "ImGui/imgui.h"
#include "Utility/History/History.h"

void RoomVolumeEditor::Init()
{
}

void RoomVolumeEditor::Update()
{
    // Do cube trace
    lastTrace = GetVolume().PosToCoord(UpdateCameraTrace());
    if (!placing)
    {
        placeStart = lastTrace.Key;
        // Begin place? 
        if ((Input::Action::Get("LM").Pressed() ||
            Input::Action::Get("RM").Pressed()))
            placing = true; 
    }

    auto& sys = ECS::Manager::Get().GetSystem<ECS::SysCubeVolume>();
    sys.DrawEditVolume(GetVolumeID(), Coord(placeStart), lastTrace);

    // End placement? 
    CHECK_RETURN(!placing)
    CHECK_RETURN(
        Input::Action::Get("LM").Down() ||
        Input::Action::Get("RM").Pressed())
    
    placing = false;
    const uint8 val = Input::Action::Get("LM").Released() ? 1 : 0;

    struct VolumeChange
    {
        ECS::ID VolumeID; 
        CubeVolumeData OriginalVolume; 
        Coord Start;
        Coord End;
    };
            
    GetHistory().AddChange(Utility::Change<VolumeChange>(
        [&](const VolumeChange& InData)
        {
            auto& localSystem = ECS::Manager::Get().GetSystem<ECS::SysCubeVolume>();
            localSystem.Set(GetVolumeID(), InData.Start, InData.End, val);
        },
        [&](const VolumeChange& InData)
        {
            if (const auto localVolume = ECS::Manager::Get().GetComponent<ECS::CubeVolume>(InData.VolumeID))
            {
                localVolume->Data = InData.OriginalVolume;
                localVolume->UpdateCache(Mat4F()); 
            }
        },
        {
            GetVolumeID(),
            GetVolume().Data,
            placeStart,
            lastTrace
        }));
}

void RoomVolumeEditor::DebugDraw(bool InIsCameraControlling)
{
    ImGui::Text("Volume editing mode"); 
    const auto size = GetVolume().Data.size();
    ImGui::Text("Blocks: %i", static_cast<int>(size));
}
