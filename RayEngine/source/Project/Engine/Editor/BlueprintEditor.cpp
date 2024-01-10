#include "BlueprintEditor.h"

#include <filesystem>

#include "Engine/Blueprints/Blueprint.h"
#include "Engine/ECS/Systems/Attributes.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Instance/Manager.h"
#include "Game/GameInstance.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_custom.h"
#include "ImGui/imgui_stdlib.h"
#include "Utility/RayUtility.h"

void BlueprintEditor::Init()
{
    Instance::Init();
    Config.LoadConfig();
    SetBP(Config.Blueprint);
}

void BlueprintEditor::Deinit()
{
    Config.SaveConfig();
}

void BlueprintEditor::SetBP(const ResBlueprint& InBP)
{
    if (InstanceID != ECS::InvalidID)
        ECS::Manager::Get().DestroyEntity(InstanceID);
    
    Config.Blueprint = InBP;
    Config.SaveConfig();
    if (BlueprintResource* bp = Config.Blueprint.Get().Get())
        InstanceID = bp->Instantiate();

    SelectedID = InstanceID; 
}

void BlueprintEditor::Update(double InDelta)
{
    Time.Tick(InDelta);
    
    // Update
    ECS.Update(Time.Delta());
    Camera.Update(InDelta);
    
    if (IsKeyDown(KEY_LEFT_CONTROL))
    {
        if (IsKeyPressed(KEY_S))
            if (BlueprintResource* bp = Config.Blueprint.Get().Get())
                bp->Save(InstanceID); 
        
        if (IsKeyPressed(KEY_P))
            Engine::Manager::Get().Push<GameInstance>();
    }
}

void BlueprintEditor::Draw()
{
}

void BlueprintEditor::DrawDebugUI()
{
    if (ImGui::Begin("Blueprint Editor"))
    {
        if (Config.Blueprint.Edit())
            SetBP(Config.Blueprint);
        ImGui::SameLine();
        if (ImGui::Button("Save"))
            if (BlueprintResource* bp = Config.Blueprint.Get().Get())
                bp->Save(InstanceID);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        EditHierarhcy(SelectedID);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        EditComponents(SelectedID); 
    }
    ImGui::End();
}

void BlueprintEditor::EditHierarhcy(ECS::EntityID InID)
{
    const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
    ImGui::Text("Hierarchy"); 
    if (ImGui::BeginSection("Hierarchy"))
        HierarchyNode(InstanceID); 
    ImGui::EndSection();
    
    if (ImGui::Button(Text("Add child").c_str()))
    {
        // Add children?
        const ECS::EntityID child = ECS.CreateEntity();
        auto& transSys = ECS.GetSystem<ECS::SysTransform>();
        transSys.Register(child, false); 
        transSys.SetupHierarchy(InID, child, ECS::Transform::Space::LOCAL, false);
    }
    
    if (InID != InstanceID)
    {
        ImGui::SameLine();
        if (ImGui::Button(Text("Remove selected").c_str()))
        {
            ECS.DestroyEntity(InID);
            SelectedID = InstanceID; 
        }
    }
}

void BlueprintEditor::HierarchyNode(ECS::EntityID InID)
{
    const auto attributes = ECS.GetComponent<ECS::Attributes>(InID);
    const auto trans = ECS.GetComponent<ECS::Transform>(InID);

    String name = "Unnamed";
    if (attributes)
        name = attributes->Name.Get(); 
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
    if (trans && trans->GetChildren().empty())
        flags |= ImGuiTreeNodeFlags_Leaf;
    flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    if (InID == SelectedID)
        flags |= ImGuiTreeNodeFlags_Selected;
    
    if (ImGui::TreeNodeEx((name + "##Node_" + std::to_string(InID)).c_str(), flags))
    {
        if (ImGui::IsItemClicked())
            SelectedID = InID;
        ImGui::Indent(10); 
        if (trans)
            for (const ECS::EntityID child : trans->GetChildren())
                HierarchyNode(child);
        ImGui::Unindent(10); 
        ImGui::TreePop(); 
    }
}

String BlueprintEditor::Text(const String& InString) const
{
    return String(InString + "##" + std::to_string(InstanceID));
}

void BlueprintEditor::EditComponents(ECS::EntityID InID)
{
    if (InID == ECS::InvalidID)
    {
        ImGui::Text("Invalid instance");
        SelectedID = InstanceID;
        return; 
    }

    ImGui::Text("Components");
    
    // Component list
    for (auto sys : ECS.GetAllSystems())
    {
        CHECK_ASSERT(!sys.second, "System nullptr");
        CHECK_CONTINUE(!sys.second->Contains(InID));
        const ImGuiTreeNodeFlags flags = 0;
        if (ImGui::CollapsingHeader(Text(sys.first).c_str(), flags))
        {
            if (ImGui::BeginSection(("Component##" + sys.first).c_str()))
            {
                sys.second->Edit(InID);
                if (ImGui::Button(Text("Remove##" + sys.first).c_str()))
                    sys.second->Unregister(InID);
            }
            ImGui::EndSection();
        }
    }
    ImGui::Spacing(); 

    // Add Component
    static int currItem = 0;
    Vector<String> newSystems;
    for (auto& sys : ECS.GetAllSystems())
    {
        CHECK_ASSERT(!sys.second, "System nullptr");
        CHECK_CONTINUE(sys.second->Contains(InID));
        newSystems.push_back(sys.first);
    }
    ImGui::Combo(Text("").c_str(), &currItem, newSystems);
    ImGui::SameLine(); 
    if (ImGui::Button(Text("Add").c_str()))
        if (currItem >= 0 && currItem < newSystems.size())
            if (const auto sys = ECS.GetSystem(newSystems[currItem]))
                sys->Register(InID);
}


