#include "RoomEditor.h"

#include "ImGui/imgui.h"

void RoomEditor::Init()
{
    CurrConfig.Load();
    if (const auto scene = CurrConfig.Scene.Get().Get())
        Scene = scene->Create();
}

void RoomEditor::Deinit()
{
    Scene.Destroy();
}

void RoomEditor::Update(double InDelta)
{
    Time.Frame(InDelta);
    
    // Update
    ECS.Update(Time.Delta());
    Camera.Update(InDelta);
}

void RoomEditor::UpdateUI()
{
    if (ImGui::Begin("Blueprint Editor"))
    {
        if (CurrConfig.Edit())
        {
            if (const auto scene = CurrConfig.Scene.Get().Get())
            {
                Scene.Destroy();
                Scene = scene->Create();
            }
        }
    }
}
