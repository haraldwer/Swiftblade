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
    LastTrace = GetVolume().PosToCoord(UpdateCameraTrace());
    if (!Placing)
    {
        PlaceStart = LastTrace.Key;
        // Begin place? 
        if ((Input::Action::Get("LM").Pressed() ||
            Input::Action::Get("RM").Pressed()))
            Placing = true; 
    }

    auto& sys = ECS::Manager::Get().GetSystem<ECS::SysCubeVolume>();
    sys.DrawEditVolume(GetVolumeID(), Coord(PlaceStart), LastTrace);

    // End placement? 
    CHECK_RETURN(!Placing)
    CHECK_RETURN(
        Input::Action::Get("LM").Down() ||
        Input::Action::Get("RM").Pressed())
    
    Placing = false;
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
            PlaceStart,
            LastTrace
        }));
}

void RoomVolumeEditor::DebugDraw(bool InIsCameraControlling)
{
    ImGui::Text("Volume editing mode"); 
    const auto size = GetVolume().Data.size();
    ImGui::Text("Blocks: %i", static_cast<int>(size));
}
