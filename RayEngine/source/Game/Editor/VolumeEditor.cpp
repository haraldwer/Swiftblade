#include "VolumeEditor.h"

#include "Engine/ECS/Manager.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Rendering/Instances/CameraInstance.h"
#include "Game/Systems/CubeVolume.h"
#include "ImGui/imgui.h"

void VolumeEditor::Init()
{
    SetCubeVolume();
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
        if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
            IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)))
        {
            Placing = true; 
        }
    }
    
    // Show volume 
    sys.DrawEditVolume(GetVolumeID(), Coord(PlaceStart), trace);

    if (Placing)
    {
        // Release to place cubes
        if (!IsMouseButtonDown(MOUSE_BUTTON_RIGHT) &&
            !IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Placing = false;
            const uint8 val = IsMouseButtonReleased(MOUSE_BUTTON_LEFT) ? 1 : 0;

            struct VolumeChange
            {
                ECS::ID VolumeID; 
                ECS::Volume OriginalVolume; 
                Coord Start;
                Coord End;
            };
            
            History.AddChange(Utility::Change<VolumeChange>(
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

    if (IsKeyDown(KEY_LEFT_CONTROL))
    {
        if (IsKeyPressed(KEY_Z))
            History.Undo();
        if (IsKeyPressed(KEY_Y))
            History.Redo();
    }
}

void VolumeEditor::UpdateUI() const
{
    ImGui::Text("Volume editing mode"); 
    const auto size = GetVolume().Data.size();
    ImGui::Text(("Blocks: " + std::to_string(size)).c_str());
}
