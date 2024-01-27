#include "VolumeEditor.h"

#include "Engine/ECS/Manager.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Rendering/Instances/CameraInstance.h"
#include "Game/ECS/Volume/CubeVolume.h"
#include "ImGui/imgui.h"

void VolumeEditor::Init()
{
    // TODO: If there is none, maybe create one?
}

void VolumeEditor::Update(double InDelta)
{
    // If in cube edit mode, add / remove cubes
    
    // Do cube trace
    auto& sys = ECS::Manager::Get().GetSystem<ECS::SysCubeVolume>();
    const CameraInstance cam = Engine::Instance::Get().GetRenderScene().GetCamera();
    const Coord trace = sys.Trace(
        GetVolumeID(),
        cam.Position,
        Mat4F(cam.Rotation).Forward(),
        static_cast<int32>(PlaceDist));
    
    if (!Placing)
    {
        PlaceStart = trace.Key;

        // Press to set target cube
        if ((Input::Action::Get("LM").Pressed() ||
            Input::Action::Get("RM").Pressed()))
        {
            Placing = true; 
        }
    }
    
    // Show volume 
    sys.DrawEditVolume(GetVolumeID(), Coord(PlaceStart), trace);

    if (Placing)
    {
        // Release to place cubes
        if (!Input::Action::Get("LM").Down() &&
            !Input::Action::Get("RM").Pressed())
        {
            Placing = false;
            const uint8 val = Input::Action::Get("LM").Released() ? 1 : 0;

            struct VolumeChange
            {
                ECS::ID VolumeID; 
                ECS::CubeVolumeData OriginalVolume; 
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
                        localVolume->Data = InData.OriginalVolume;
                },
                {
                    GetVolumeID(),
                    GetVolume().Data,
                    PlaceStart,
                    trace
                }));
        }
    }
}

void VolumeEditor::UpdateUI(bool InIsCameraControlling)
{
}

void VolumeEditor::DebugDraw(bool InIsCameraControlling)
{
    ImGui::Text("Volume editing mode"); 
    const auto size = GetVolume().Data.size();
    ImGui::Text(("Blocks: " + std::to_string(size)).c_str());
}
