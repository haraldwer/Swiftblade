#include "Manager.h"

#include "Impl.h"
#include "ImGui/imgui.h"

Resource::Base* Resource::Manager::GetResource(const String& InIdentifier)
{
    const auto find = Resources.find(InIdentifier);
    if (find == Resources.end())
        return nullptr; 
    return find->second;
}

void Resource::Manager::Register(Base* InResource, const String& InIdentifier)
{
    Resources[InIdentifier] = InResource;
}

void Resource::Manager::Update()
{
    // Only check every 3 seconds
    CHECK_RETURN(CheckTimer.Ellapsed() < CheckInterval)
    
    PROFILE();
    CheckTimer = Utility::Timer();
    TryUnload();
}

void Resource::Manager::TryUnload() const
{
    PROFILE();
    Vector<String> queue;
    queue.reserve(Resources.size());
    for (const auto& res : Resources)
        queue.push_back(res.first);

    for (int i = 0; i < Utility::Math::Min(static_cast<int>(queue.size()), CheckNum); i++)
    {
        static int index = 0;
        index = (index + 1) % static_cast<int>(queue.size());
        
        auto res = Resources.at(queue[index]);
        CHECK_CONTINUE(!res)
        CHECK_CONTINUE(!res->Loaded)

        if (res->Count <= 0)
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
    for (auto& res : Resources)
    {
        CHECK_CONTINUE(!res.second)
        res.second->Unload();
        CHECK_CONTINUE_LOG(res.second->Count != 0, ("Resource couldnt be destroyed properly, count: " + std::to_string(res.second->Count) + ", resource: " + res.first));
        delete(res.second);
        res.second = nullptr;
    }
    Resources.clear();
}

void Resource::Manager::DrawDebugWindow()
{
    ImGui::Text(String("Total resources: " + std::to_string(Resources.size())).c_str());

    int c = 0;
    for (auto r : Resources)
        if (r.second->Loaded)
            c++; 
    ImGui::Text(String("Loaded resources: " + std::to_string(c)).c_str());

    static bool showOnlyLoaded = true;
    ImGui::Checkbox("Show only loaded", &showOnlyLoaded); 
    
    if (ImGui::BeginTable("Resources", 3, ImGuiTableFlags_Borders))
    {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Count", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Loaded", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();
        
        for (const auto& res : Resources)
        {
            CHECK_CONTINUE(showOnlyLoaded && !res.second->Loaded);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text(res.first.c_str());
            ImGui::TableNextColumn();
            CHECK_CONTINUE(!res.second); 
            ImGui::Text(std::to_string(res.second->Count).c_str());
            ImGui::TableNextColumn();
            ImGui::Text(res.second->Loaded ? "True" : "False");
        }
        ImGui::EndTable(); 
    }
}

