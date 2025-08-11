#include "BlueprintEditor.h"

#include <filesystem>

#include "Blueprints/Blueprint.h"
#include "ECS/Systems/Attributes.h"
#include "ECS/Systems/Transform.h"
#include "Instance/Manager.h"
#include "../../Game/Instances/GameInstance.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_custom.h"
#include "ImGui/imgui_stdlib.h"

void BlueprintEditor::Init()
{
    Instance::Init();
    ecs.Init();
    editorCamera.Toggle(); 
    config.LoadConfig();
    SetBP(config.Blueprint);
}

void BlueprintEditor::Deinit()
{
    Instance::Deinit();
    ecs.Deinit();
    config.SaveConfig();
}

void BlueprintEditor::SetBP(const ResBlueprint& InBP)
{
    if (instanceID != ECS::INVALID_ID)
        ECS::Manager::Get().DestroyEntity(instanceID);
    
    config.Blueprint = InBP;
    config.SaveConfig();
    if (const BlueprintResource* bp = config.Blueprint.Get().Get())
        instanceID = bp->Instantiate();

    selectedID = instanceID;

    if (const auto t = ecs.GetComponent<ECS::Transform>(instanceID))
        editorCamera.SetTarget(t->GetPosition());
}

void BlueprintEditor::Logic(const double InDelta)
{
    Instance::Logic(InDelta);

    // Update
    ecs.Update();
    editorCamera.Update();

    if (instanceID == ECS::INVALID_ID)
        return; 
    
    if (Input::Action::Get("Ctrl").Down())
    {
        if (Input::Action::Get("Save").Pressed())
            if (BlueprintResource* bp = config.Blueprint.Get().Get())
                bp->Save(instanceID); 
        
        if (Input::Action::Get("Play").Pressed())
            Engine::Manager::Get().Push<GameInstance>();
    }
}

void BlueprintEditor::Frame()
{
    ecs.Frame(); 
    Instance::Frame();
}

void BlueprintEditor::DrawDebugPanel()
{
    if (config.Blueprint.Edit())
        SetBP(config.Blueprint);

    if (instanceID == ECS::INVALID_ID)
        return; 
    
    ImGui::SameLine();
    if (ImGui::Button("Save"))
        if (BlueprintResource* bp = config.Blueprint.Get().Get())
            bp->Save(instanceID);

    EditHierarhcy(selectedID);
    EditComponents(selectedID);
}

void BlueprintEditor::EditHierarhcy(ECS::EntityID InID)
{
    ImGui::SeparatorText("Hierarchy"); 
    {
        HierarchyNode(instanceID);
        ImGui::Spacing(); 
        
        if (ImGui::Button(Text("Add child").c_str()))
        {
            // Add children?
            const ECS::EntityID child = ecs.CreateEntity();
            auto& transSys = ecs.GetSystem<ECS::SysTransform>();
            transSys.Register(child, false); 
            transSys.SetupHierarchy(InID, child, ECS::Transform::Space::LOCAL, false);
        }
        
        if (InID != instanceID)
        {
            ImGui::SameLine();
            if (ImGui::Button(Text("Remove selected").c_str()))
            {
                ecs.DestroyEntity(InID);
                selectedID = instanceID; 
            }
        }
    } 
}

void BlueprintEditor::HierarchyNode(ECS::EntityID InID)
{
    const auto attributes = ecs.GetComponent<ECS::Attributes>(InID);
    const auto trans = ecs.GetComponent<ECS::Transform>(InID);

    String name = "Unnamed";
    if (attributes)
        name = attributes->Name.Get(); 
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
    if (trans && trans->GetChildren().empty())
        flags |= ImGuiTreeNodeFlags_Leaf;
    flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    if (InID == selectedID)
        flags |= ImGuiTreeNodeFlags_Selected;
    
    if (ImGui::TreeNodeEx((name + "##Node_" + std::to_string(InID)).c_str(), flags))
    {
        if (ImGui::IsItemClicked())
            selectedID = InID;
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
    return String(InString + "##" + std::to_string(instanceID));
}

void BlueprintEditor::EditComponents(ECS::EntityID InID)
{
    ImGui::SeparatorText("Components");
    
    if (InID == ECS::INVALID_ID)
    {
        ImGui::Text("Invalid instance");
        selectedID = instanceID;
        return; 
    }
    
    // Component list
    for (auto sys : ecs.GetAllSystems())
    {
        CHECK_ASSERT(!sys.second, "System nullptr");
        CHECK_CONTINUE(!sys.second->Contains(InID));
        const ImGuiTreeNodeFlags flags = 0;
        if (ImGui::CollapsingHeader(Text(sys.first).c_str(), flags))
        {
            if (ImGui::BeginSection(false))
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
    for (auto& sys : ecs.GetAllSystems())
    {
        CHECK_ASSERT(!sys.second, "System nullptr");
        CHECK_CONTINUE(sys.second->Contains(InID));
        newSystems.push_back(sys.first);
    }
    ImGui::Combo(Text("").c_str(), &currItem, newSystems);
    ImGui::SameLine(); 
    if (ImGui::Button(Text("Add").c_str()))
        if (currItem >= 0 && currItem < static_cast<int>(newSystems.size()))
            if (const auto sys = ecs.GetSystem(newSystems[currItem]))
                sys->Register(InID);
}


