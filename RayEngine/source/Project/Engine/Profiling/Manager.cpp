#include "Manager.h"

#include "ImGui/imgui.h"
#include "Utility/Hash.h"

void Profiling::Manager::Frame()
{
    PreviousScopes = Scopes;
    PreviousFrameTime = FrameTimer.Ellapsed();
    FrameTimer = Utility::Timer();
    Scopes.clear();
    ActiveScopes.clear();
}

void Profiling::Manager::Begin(const String& InName, const String& InFunction)
{
    const uint32 hash = Utility::Hash(InName + InFunction);
    auto& scope = Scopes[hash];
    if (scope.Calls == 0)
    {
        scope.Name = InName;
        scope.Function = InFunction;
        scope.Depth = ActiveScopes.size();
    }
    scope.Timer = Utility::Timer(); 
    ActiveScopes.push_back(hash); 
}

void Profiling::Manager::End()
{
    CHECK_ASSERT(ActiveScopes.empty(), "Scope not active")
    const uint32 hash = ActiveScopes.back();
    ActiveScopes.pop_back();
    auto& data = Scopes[hash];
    data.TotalTime += data.Timer.Ellapsed();
    data.Calls++;
}

void Profiling::Manager::DrawDebugWindow()
{
    ImGui::Text(("Total duration (ms): " + std::to_string(PreviousFrameTime * 1000.0)).c_str());
    if (ImGui::BeginTable("Scopes", 6, ImGuiTableFlags_Borders))
    {
        ImGui::TableSetupColumn("Depth", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Function");
        ImGui::TableSetupColumn("Calls", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("%", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("ms", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        // Sorted insert
        Vector<uint32> sorted;
        for (auto& entry : PreviousScopes)
            sorted.insert( 
               std::ranges::upper_bound(sorted, entry.first, [&](const uint32 InFirst, const uint32 InSecond)
               {
                   return PreviousScopes.at(InFirst).TotalTime > PreviousScopes.at(InSecond).TotalTime;
               }),
               entry.first);

        for (auto& hash : sorted)
        {
            auto& entry = PreviousScopes.at(hash);
            
            CHECK_CONTINUE(entry.Calls == 0);
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text(std::to_string(entry.Depth).c_str());
            
            ImGui::TableNextColumn();
            ImGui::Text(entry.Name.c_str());

            ImGui::TableNextColumn();
            ImGui::Text(entry.Function.c_str());

            ImGui::TableNextColumn();
            ImGui::Text(std::to_string(entry.Calls).c_str());

            ImGui::TableNextColumn();
            ImGui::Text(std::to_string((entry.TotalTime / PreviousFrameTime) * 100.0).substr(0, 4).c_str());

            ImGui::TableNextColumn();
            ImGui::Text(std::to_string(entry.TotalTime * 1000.0).substr(0, 4).c_str());

        }
        ImGui::EndTable(); 
    }
}
