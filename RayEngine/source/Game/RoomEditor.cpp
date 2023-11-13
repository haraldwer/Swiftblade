#include "RoomEditor.h"

#include "Game.h"
#include "Engine/Instance/Manager.h"
#include "ImGui/imgui.h"
#include "Systems/CubeVolume.h"

void RoomEditor::Init()
{
    Instance::Init();
    CurrConfig.LoadConfig();
    
    if (const auto scene = CurrConfig.Scene.Get().Get())
        Scene = scene->Create();

    const auto& sys = ECS.GetSystem<ECS::SysCubeVolume>();
    const auto volumes = sys.GetEntities();
    CHECK_RETURN_LOG(volumes.empty(), "No cube volume");
    CubeVolume = volumes[0];

    Camera.SetRequireHold(false);  
}

void RoomEditor::Deinit()
{
    CurrConfig.SaveConfig(); 
    Scene.Destroy();
    Instance::Deinit();
}

void RoomEditor::Update(double InDelta)
{
    Time.Frame(InDelta);
    
    // Update
    ECS.Update(Time.Delta());
    Camera.Update(InDelta);

    if (IsKeyPressed(KEY_TAB))
        Camera.ToggleRequireHold();

    if (Camera.IsControlling())
    {
        // If in cube edit mode, add / remove cubes
        CHECK_RETURN(CubeVolume == ECS::InvalidID)
        // Change place dist
        PlaceDist = CLAMP(0.0f, 10.0f, PlaceDist + GetMouseWheelMove());
        // Do cube trace
        auto& sys = ECS.GetSystem<ECS::SysCubeVolume>();
        const CameraInstance cam = GetRenderScene().GetCamera();
        const Coord trace = sys.Trace(CubeVolume, cam.Position, Mat4F(cam.Rotation).Forward(), static_cast<int32>(PlaceDist));

        // Press to set target cube

        if (!Placing &&
            (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
            IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)))
        {
            Placing = true; 
            PlaceStart = trace.Key;
        }

        if (Placing)
        {
            if (!IsMouseButtonDown(MOUSE_BUTTON_RIGHT) &&
                !IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            {
                Placing = false;
                const uint8 val = IsMouseButtonReleased(MOUSE_BUTTON_LEFT) ? 1 : 0;
                sys.Set(CubeVolume, PlaceStart, trace, val);
            }
            
            // Should probably draw selection too
        }
        
        // If in object mode, place / remove selected object type
    }
    else if (IsKeyDown(KEY_LEFT_CONTROL))
    {
        if (IsKeyPressed(KEY_S))
        {
            // TODO: Save
        }
        if (IsKeyPressed(KEY_P))
        {
            // Create game instance
            Engine::Manager::Get().Push<Game>();
        }
    }
}

void RoomEditor::UpdateUI()
{
    if (ImGui::Begin("Room Editor"))
    {
        if (CurrConfig.Edit())
        {
            if (const auto scene = CurrConfig.Scene.Get().Get())
            {
                Scene.Destroy();
                Scene = scene->Create();
            }
        }

        if (CubeVolume != ECS::InvalidID)
        {
            ImGui::Text(("Entities: " + std::to_string(Scene.Entities.size())).c_str());

            auto& sys = ECS.GetSystem<ECS::SysCubeVolume>();
            const auto size = sys.Get<ECS::CubeVolume>(CubeVolume).Data.Get().size();
            ImGui::Text(("Blocks: " + std::to_string(size)).c_str());
        }
        
        if (ImGui::Button("Save"))
        {
            // TODO: Save the scene, with overrides!
        }
    }
    ImGui::End(); 
}
