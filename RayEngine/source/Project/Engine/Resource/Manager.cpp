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
    CHECK_RETURN(CheckTimer.Ellapsed() < 1.0f)
    CheckTimer = Utility::Timer();
    HotReload(1); 
}

void Resource::Manager::HotReload(int InNum) const
{
    int count = 0; 
    for (const auto& res : Resources)
    {
        CHECK_CONTINUE(!res.second)
        CHECK_CONTINUE(!res.second->Loaded)

        // Maybe unload? 
        if (res.second->Count <= 0)
        {
            res.second->Unload();
            continue; 
        }

        // Maybe reload? 
        if (res.second->TryHotReload())
        {
            count++;
            if (InNum > 0 && count >= InNum)
                break; // Only reload a couple at a time
        }
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
    if (ImGui::Begin("Resources"))
    {
        ImGui::Text(String("Total: " + std::to_string(Resources.size())).c_str());
        if (ImGui::BeginTable("Resources", 3, ImGuiTableFlags_Borders))
        {
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Count", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Loaded", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();
            
            for (const auto& res : Resources)
            {
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
    ImGui::End();
}

