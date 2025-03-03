#include "BlueprintEditor.h"

#include <filesystem>

#include "Blueprints/Blueprint.h"
#include "ECS/Systems/Attributes.h"
#include "ECS/Systems/Transform.h"
#include "Instance/Manager.h"
#include "Game/GameInstance.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_custom.h"
#include "ImGui/imgui_stdlib.h"

void BlueprintEditor::Init()
{
    Instance::Init();
    ECS.Init();
    EditorCamera.Toggle(); 
    Config.LoadConfig();
    SetBP(Config.Blueprint);
}

void BlueprintEditor::Deinit()
{
    Instance::Deinit();
    ECS.Deinit();
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

    if (auto t = ECS.GetComponent<ECS::Transform>(InstanceID))
        EditorCamera.SetTarget(t->GetPosition());
}

void BlueprintEditor::Logic(double InDelta)
{
    Instance::Logic(InDelta);

    // Update
    ECS.Update();
    EditorCamera.Update();

    if (InstanceID == ECS::InvalidID)
        return; 
    
    if (Input::Action::Get("Ctrl").Down())
    {
        if (Input::Action::Get("Save").Pressed())
            if (BlueprintResource* bp = Config.Blueprint.Get().Get())
                bp->Save(InstanceID); 
        
        if (Input::Action::Get("Play").Pressed())
            Engine::Manager::Get().Push<GameInstance>();
    }
}

void BlueprintEditor::Frame()
{
    ECS.Frame(); 
    Instance::Frame();
}

void BlueprintEditor::DrawDebugWindow()
{
    if (Config.Blueprint.Edit())
        SetBP(Config.Blueprint);

    if (InstanceID == ECS::InvalidID)
        return; 
    
    ImGui::SameLine();
    if (ImGui::Button("Save"))
        if (BlueprintResource* bp = Config.Blueprint.Get().Get())
            bp->Save(InstanceID);

    EditHierarhcy(SelectedID);
    EditComponents(SelectedID);
}

void BlueprintEditor::EditHierarhcy(ECS::EntityID InID)
{
    ImGui::SeparatorText("Hierarchy"); 
    {
        HierarchyNode(InstanceID);
        ImGui::Spacing(); 
        
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
    ImGui::SeparatorText("Components");
    
    if (InID == ECS::InvalidID)
    {
        ImGui::Text("Invalid instance");
        SelectedID = InstanceID;
        return; 
    }
    
    // Component list
    for (auto sys : ECS.GetAllSystems())
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


