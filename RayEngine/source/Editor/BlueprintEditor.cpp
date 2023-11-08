#include "BlueprintEditor.h"

#include <filesystem>

#include "Engine/Blueprints/Blueprint.h"
#include "Engine/Instance/Manager.h"
#include "Game/Game.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_stdlib.h"
#include "ImGui/FileBrowser/ImGuiFileBrowser.h"
#include "Utility/String.h"

void BlueprintEditor::Init()
{
    Instance::Init();
    SetBP(ResBlueprint("Blueprints/BP_Player.json"));
}

void BlueprintEditor::SetBP(const ResBlueprint& InBP)
{
    if (InstanceID != ECS::InvalidID)
        ECS::Manager::Get().DestroyEntity(InstanceID);
    
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
        if (Blueprint.Edit("BP"))
            SetBP(Blueprint);
        ImGui::SameLine();
        if (ImGui::Button("Save"))
            if (BlueprintResource* bp = Blueprint.Get())
                bp->Serialize(InstanceID);
        
        if (InstanceID == ECS::InvalidID)
        {
            ImGui::Text("Invalid instance");
        }
        else
        {
            ImGui::Text(String("ID: " + std::to_string(InstanceID)).c_str());
            for (auto sys : ECS.GetAllSystems())
            {
                CHECK_ASSERT(!sys.second, "System nullptr");
                CHECK_CONTINUE(!sys.second->Contains(InstanceID));
                if (ImGui::CollapsingHeader(sys.first.c_str()))
                    sys.second->Edit(InstanceID);
                
            }
        }
        
        static int currItem = 0;
        Vector<String> newSystems;
        for (auto& sys : ECS.GetAllSystems())
        {
            CHECK_ASSERT(!sys.second, "System nullptr");
            CHECK_CONTINUE(sys.second->Contains(InstanceID));
            newSystems.push_back(sys.first);
        }
        ImGui::Combo("##", &currItem, newSystems);
        ImGui::SameLine(); 
        
        if (ImGui::Button("Add"))
            if (currItem >= 0 && currItem < newSystems.size())
                if (const auto sys = ECS.GetSystem(newSystems[currItem]))
                    sys->Register(InstanceID);
    }
    ImGui::End();
}
