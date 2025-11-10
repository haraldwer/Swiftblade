#pragma once

#include "ECS/Entity.h"
#include "ECS/Manager.h"
#include "ECS/Systems/Attributes.h"
#include "ECS/Systems/Transform.h"
#include "ImGui/imgui_custom.h"

inline void EditHierarchy(const ECS::EntityID InRootID, ECS::EntityID& InOutSelected, bool InRecursive = true)
{
    if (InRootID == ECS::INVALID_ID)
        return;
    
    auto& man = ECS::Manager::Get();
    const auto attributes = man.GetComponent<ECS::Attributes>(InRootID);
    const auto trans = man.GetComponent<ECS::Transform>(InRootID);

    String name = "Unnamed";
    if (attributes)
        name = attributes->Name.Get(); 
    
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
    if ((trans && trans->GetChildren().empty()) || !InRecursive)
        flags |= ImGuiTreeNodeFlags_Leaf;
    flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    if (InRootID == InOutSelected)
        flags |= ImGuiTreeNodeFlags_Selected;
    
    if (ImGui::TreeNodeEx((name + "##Node_" + std::to_string(InRootID)).c_str(), flags))
    {
        if (ImGui::IsItemClicked())
            InOutSelected = InRootID;
        if (trans && InRecursive)
            for (const ECS::EntityID child : trans->GetChildren())
                EditHierarchy(child, InOutSelected);

        // Also, right click menu for removing entity?
        
        ImGui::TreePop(); 
    }
}

inline void EditScene(ECS::EntityID& InOutSelected)
{
    auto& man = ECS::Manager::Get();
    auto& transSys = man.GetSystem<ECS::SysTransform>();
    for (auto e : man.GetAllEntities())
    { 
        auto trans = transSys.TryGet(e);
        CHECK_CONTINUE(trans && trans->GetParent() != ECS::INVALID_ID)
        EditHierarchy(e, InOutSelected, false);
    } 
}

inline void EditComponents(const ECS::EntityID InID)
{
    if (InID == ECS::INVALID_ID)
        return; 

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
        if (ImGui::CollapsingHeader(text(sys.first, InID).c_str()))
        {
            ImGui::Spacing();
            if (ImGui::BeginSection(false))
            {
                sys.second->Edit(InID);
                if (ImGui::Button(text("Remove##" + sys.first, InID).c_str(), ImVec2(-1, 0)))
                    sys.second->Unregister(InID);
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
        if (currItem >= 0 && currItem < static_cast<int>(newSystems.size()))
            if (const auto sys = ecs.GetSystem(newSystems[currItem]))
                sys->Register(InID);
}
