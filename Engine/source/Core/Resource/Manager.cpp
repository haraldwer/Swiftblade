#include "Manager.h"

#include "Impl.h"
#include "Utility/Collections/SortedInsert.h"
#include "ImGui/imgui.h"

Resource::ImplBase* Resource::Manager::GetResource(const ID& InID)
{
    const auto find = resources.find(InID.Hash());
    if (find == resources.end())
        return nullptr; 
    return find->second;
}

void Resource::Manager::Register(ImplBase* InResource, const ID& InID)
{
    resources[InID.Hash()] = InResource;
}

void Resource::Manager::Frame()
{
    // Only check every 3 seconds
    CHECK_RETURN(checkTimer.Ellapsed() < CHECK_INTERVAL)
    
    PROFILE();
    checkTimer = Utility::Timer();
    TryUnload();
}

void Resource::Manager::TryUnload() const
{
    PROFILE();
    Vector<uint32> queue;
    queue.reserve(resources.size());
    for (const auto& res : resources)
        queue.push_back(res.first);

    for (int i = 0; i < Utility::Math::Min(static_cast<int>(queue.size()), CHECK_NUM); i++)
    {
        static size_t index = 0;
        index = (index + 1) % static_cast<int>(queue.size());

        const auto res = resources.at(queue[index]);
        CHECK_CONTINUE(!res)
        CHECK_CONTINUE(!res->loaded)
        if (res->count <= 0)
        {
            PROFILE_NAMED("Unload");
            res->Unload();
        }
#ifdef _DEBUG
#ifndef __EMSCRIPTEN__
        else
        {
            PROFILE_NAMED("Hot reload");
            res->TryHotReload();
        }
#endif
#endif
    }    
}

void Resource::Manager::Deinit()
{
    // TODO: Unload all
    for (auto& res : resources)
    {
        CHECK_CONTINUE(!res.second)
        res.second->Unload();
        CHECK_CONTINUE_LOG(res.second->count != 0, "Resource couldnt be destroyed properly, count: " + std::to_string(res.second->count) + ", resource: " + res.second->id.Str());
        delete(res.second);
        res.second = nullptr;
    }
    resources.clear();
}

#ifdef IMGUI_ENABLE

void Resource::Manager::DrawPanel()
{
    ImGui::Text("Total resources: %i", static_cast<int>(resources.size()));

    int c = 0;
    for (auto r : resources)
        if (r.second->loaded)
            c++; 
    ImGui::Text("Loaded resources: %i", c);

    static bool showOnlyLoaded = true;
    ImGui::Checkbox("Show only loaded", &showOnlyLoaded);
    
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
    if (ImGui::BeginTable("Resources", 3, ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_RowBg))
    {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Count", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Loaded", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        Vector<uint32> sorted;
        for (const auto& res : resources)
            Utility::SortedInsert(sorted, res.first, [&](const uint32 InA, const uint32 InB)
            {
                return resources.at(InA)->id.Str().compare(resources.at(InB)->id.Str()) < 0;
            });
        
        for (const auto& id : sorted)
        {
            auto& res = resources.at(id);
            CHECK_CONTINUE(showOnlyLoaded && !res->loaded);
            CHECK_CONTINUE(!res); 
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (ImGui::Selectable((res->id.Str() +"##" + Utility::ToStr(id)).c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                pickCallback(res->id.Str());
            ImGui::TableNextColumn();
            ImGui::Text("%i", res->count);
            ImGui::TableNextColumn();
            ImGui::Text(res->loaded ? "True" : "False");
        }
        ImGui::PopStyleVar();
        ImGui::EndTable(); 
    }
}

#else

void Resource::Manager::DrawDebugPanel() {}

#endif
