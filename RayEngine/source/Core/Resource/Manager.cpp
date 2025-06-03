#include "Manager.h"

#include "Impl.h"
#include "ImGui/imgui.h"

Resource::Base* Resource::Manager::GetResource(const String& InIdentifier)
{
    const auto find = resources.find(InIdentifier);
    if (find == resources.end())
        return nullptr; 
    return find->second;
}

void Resource::Manager::Register(Base* InResource, const String& InIdentifier)
{
    resources[InIdentifier] = InResource;
}

void Resource::Manager::Update()
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
    Vector<String> queue;
    queue.reserve(resources.size());
    for (const auto& res : resources)
        queue.push_back(res.first);

    for (int i = 0; i < Utility::Math::Min(static_cast<int>(queue.size()), CHECK_NUM); i++)
    {
        static int index = 0;
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
        else
        {
            PROFILE_NAMED("Hot reload");
            res->TryHotReload();
        }
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
        CHECK_CONTINUE_LOG(res.second->count != 0, "Resource couldnt be destroyed properly, count: " + std::to_string(res.second->count) + ", resource: " + res.first);
        delete(res.second);
        res.second = nullptr;
    }
    resources.clear();
}

void Resource::Manager::DrawDebugPanel()
{
    ImGui::Text("Total resources: %i", static_cast<int>(resources.size()));

    int c = 0;
    for (auto r : resources)
        if (r.second->loaded)
            c++; 
    ImGui::Text("Loaded resources: %i", c);

    static bool showOnlyLoaded = true;
    ImGui::Checkbox("Show only loaded", &showOnlyLoaded); 
    
    if (ImGui::BeginTable("Resources", 3, ImGuiTableFlags_Borders))
    {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Count", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Loaded", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();
        
        for (const auto& res : resources)
        {
            CHECK_CONTINUE(showOnlyLoaded && !res.second->loaded);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", res.first.c_str());
            ImGui::TableNextColumn();
            CHECK_CONTINUE(!res.second); 
            ImGui::Text("%i", res.second->count);
            ImGui::TableNextColumn();
            ImGui::Text(res.second->loaded ? "True" : "False");
        }
        ImGui::EndTable(); 
    }
}

