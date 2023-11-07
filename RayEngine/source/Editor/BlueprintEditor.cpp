#include "BlueprintEditor.h"

#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Instance/Manager.h"
#include "Game/Game.h"
#include "ImGui/imgui.h"

void BlueprintEditor::Init()
{
    Instance::Init();
    SetBP(ResBlueprint("player.json"));
}

void BlueprintEditor::SetBP(const ResBlueprint& InBP)
{
    Blueprint = InBP;
    if (BlueprintResource* bp = Blueprint.Get())
        InstanceID = bp->Instantiate();
}

void BlueprintEditor::Update(double InDelta)
{
    Time.Frame(InDelta);
    
    // Update
    ECS.Update(Time.Delta());
    Camera.Update(InDelta);
    
    if (IsKeyDown(KEY_LEFT_CONTROL))
    {
        if (IsKeyPressed(KEY_S))
            if (BlueprintResource* bp = Blueprint.Get())
                bp->Serialize(InstanceID); 
        
        if (IsKeyPressed(KEY_P))
            Engine::Manager::Get().Push<Game>();
    }
}

void BlueprintEditor::UpdateUI()
{
    if (ImGui::Begin("Blueprint Editor"))
    {
        ImGui::Text(String("BP: " + Blueprint.Identifier()).c_str());
        if (InstanceID == ECS::InvalidID)
        {
            ImGui::Text("Invalid instance");
        }
        else
        {
            ImGui::Text(String("ID: " + std::to_string(InstanceID)).c_str());
            for (auto sys : ECS.GetAllSystems())
            {
                CHECK_CONTINUE(!sys.second);
                CHECK_CONTINUE(!sys.second->Contains(InstanceID));
                if (ImGui::CollapsingHeader(sys.first.c_str()))
                    sys.second->Edit(InstanceID);
                
            }
        }
    }
    ImGui::End(); 
}
