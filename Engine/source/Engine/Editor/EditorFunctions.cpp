#include "EditorFunctions.h"

#ifdef IMGUI_ENABLE

#include "ImGui/imgui.h"
#include "ImGui/imgui_stdlib.h"
#include "ImGui/imgui_custom.h"

#include "ECS/Manager.h"
#include "ECS/Systems/Attributes.h"
#include "ECS/Systems/Transform.h"
#include "Instance/Instance.h"

inline bool HierarchyNode(const ECS::EntityID InRootID, ECS::EntityID& InOutSelected, const bool InRecursive)
{
    if (InRootID == ECS::INVALID_ID)
        return false;

    bool result = false;
    
    auto& man = ECS::Manager::Get();
    const auto attributes = man.GetComponent<ECS::Attributes>(InRootID);
    const auto trans = man.GetComponent<ECS::Transform>(InRootID);
    
    String name = "Unnamed";
    if (attributes)
        name = attributes->Name.Get(); 

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    
    float height = ImGui::CalcTextSize("T").y + 2;
    String id = name + "##Node_" + std::to_string(InRootID);
    if (ImGui::Selectable(id.c_str(), InOutSelected == InRootID, ImGuiSelectableFlags_SpanAllColumns, ImVec2(0, height)))
    {
        InOutSelected = InRootID;
        LOG("Hierarchy selection: " + name);
    }
    
    if (ImGui::BeginPopupContextItem(id.c_str()))
    {
        InOutSelected = InRootID;

        if (trans)
        {
            if (ImGui::Selectable("Add child##Hierarchy"))
            {
                ECS::EntityID newID = man.CreateEntity();
                man.GetSystem<ECS::SysTransform>().Register(newID);
                trans->AddChild(newID);
                InOutSelected = newID;
                result = true;
                LOG("Child added");
            }
        }

        if (ImGui::Selectable("Remove##Hierarchy"))
        {
            if (trans) InOutSelected = trans->GetParent();
            man.DestroyEntity(InRootID);
            result = true;
            LOG("Entity removed")
        }
        
        ImGui::EndPopup();
    }
    
    ImGui::Indent();
    if (trans && InRecursive)
    {
        auto children = trans->GetChildren();
        for (const ECS::EntityID child : children)
            result |= HierarchyNode(child, InOutSelected, true);
    }
    ImGui::Unindent();
    
    return result;
}

bool EditHierarchy(const ECS::EntityID InRootID, ECS::EntityID& InOutSelected, bool InRecursive)
{
    bool result = false; 
    if (ImGui::BeginTable(("EditHierarchy" + Utility::ToStr(InRootID)).c_str(), 1, ImGuiTableFlags_BordersOuter))
    {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        result |= HierarchyNode(InRootID, InOutSelected, InRecursive);        
        ImGui::EndTable();
    }
    return result;
}

bool EditScene(ECS::EntityID& InOutSelected)
{
    bool result = false;
    if (ImGui::BeginTable("EditScene", 1, ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersOuter))
    {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        
        auto& man = ECS::Manager::Get();
        auto& transSys = man.GetSystem<ECS::SysTransform>();
        for (auto e : man.GetAllEntities())
        { 
            auto trans = transSys.TryGet(e);
            CHECK_CONTINUE(trans && trans->GetParent() != ECS::INVALID_ID)
            result |= HierarchyNode(e, InOutSelected, false);
        }
        ImGui::EndTable();
    }
    return result;
}

bool EditComponents(const ECS::EntityID InID)
{
    if (InID == ECS::INVALID_ID)
        return false; 

    bool result = false;
    
    auto& ecs = ECS::Manager::Get();
    auto text = [](const String& InString, const ECS::EntityID InVal)
    {
        return String(InString + "##" + Utility::ToStr(InVal));
    };
    
    // Component list
    for (auto sys : ecs.GetAllSystems())
    {
        CHECK_ASSERT(!sys.second, "System nullptr");
        CHECK_CONTINUE(!sys.second->Contains(InID));

        ImGuiTreeNodeFlags flags = 0;
        if (sys.first == "Transform")
            flags |= ImGuiTreeNodeFlags_DefaultOpen;
        
        if (ImGui::CollapsingHeader(text(sys.first, InID).c_str(), flags))
        {
            ImGui::Spacing();
            if (ImGui::BeginSection(false))
            {
                result |= sys.second->Edit(InID);
                if (ImGui::Button(text("Remove##" + sys.first, InID).c_str(), ImVec2(-1, 0)))
                {
                    sys.second->Unregister(InID);
                    result = true;
                    LOG("Component removed: " + sys.first);
                }
            }
            ImGui::EndSection();
            ImGui::Spacing();
            ImGui::Spacing();
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
    ImGui::Combo(text("", InID).c_str(), &currItem, newSystems);
    ImGui::SameLine(); 
    if (ImGui::Button(text("Add", InID).c_str(), ImVec2(-1, 0)))
    {
        if (currItem >= 0 && currItem < static_cast<int>(newSystems.size()))
        {
            if (const auto sys = ecs.GetSystem(newSystems[currItem]))
            {
                sys->Register(InID);
                result = true;
                LOG("Component added: " + newSystems[currItem]);
            }
        }
    }

    return result;
}

#else

void EditHierarchy(const ECS::EntityID InRootID, ECS::EntityID& InOutSelected, bool InRecursive = true) {}
void EditScene(ECS::EntityID& InOutSelected) {}
void EditComponents(const ECS::EntityID InID) {}

#endif


