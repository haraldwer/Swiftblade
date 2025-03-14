#include "Manager.h"

#include <algorithm>

#include "ImGui/imgui.h"

void Profiling::Manager::Frame()
{
    FrameTimer = Utility::Timer();
    Scopes.clear();
    ActiveScopes.clear();
}

void Profiling::Manager::Begin(const String& InName, const String& InFunction)
{
    CHECK_RETURN(Paused);
    CHECK_RETURN(!IsDebugWindowOpen());
    const uint32 hash = Utility::Hash(InName + InFunction);
    auto& scope = Scopes[hash];
    if (scope.Calls == 0)
    {
        scope.Name = InName;
        scope.Function = InFunction;
        scope.Depth = ActiveScopes.size();
        scope.ExecutionOrder = Scopes.size() - 1;
    }
    scope.Timer = Utility::Timer(); 
    ActiveScopes.push_back(hash); 
}

void Profiling::Manager::End()
{
    CHECK_RETURN(Paused);
    CHECK_RETURN(!IsDebugWindowOpen());
    CHECK_RETURN_LOG(ActiveScopes.empty(), "Scope not active")
    const uint32 hash = ActiveScopes.back();
    ActiveScopes.pop_back();
    auto& data = Scopes[hash];
    data.TotalTime += data.Timer.Ellapsed();
    data.Calls++;
}

void Profiling::Manager::DrawDebugWindow()
{
    if (!Paused && UpdateStatTimer.Ellapsed() > StatRefreshTime)
    {
        PreviousScopes = Scopes;
        PreviousFrameTime = FrameTimer.Ellapsed();
        UpdateStatTimer = Utility::Timer();
        UpdateCachedList();
    }
    
    if (!Paused)
        Histogram.push_back(static_cast<float>(FrameTimer.Ellapsed() * 1000.0f));
    
    float age = 0.0f;
    int toBeRemoved = 0;
    float max = 0.0f; 
    float min = 9999.0f;
    float avg = 0.0f; 
    for (int i = static_cast<int>(Histogram.size()) - 1; i >= 0; i--)
    {
        float h = Histogram[i]; 
        age += h / 1000.0f;
        if (age > StatHistogramDuration && toBeRemoved == 0)
            toBeRemoved = i;
        if (h > max)
            max = h;
        if (h < min)
            min = h;
        avg += h; 
    }

    if (!Histogram.empty())
        avg /= static_cast<int>(Histogram.size());
    if (!Paused && toBeRemoved > 0)
        Histogram.erase(Histogram.begin(), Histogram.begin() + toBeRemoved);
    
    const float lerp = Utility::Math::Min(static_cast<float>(FrameTimer.Ellapsed()) * 4.0f, 1.0f);
    SmoothMax = Utility::Math::Lerp(SmoothMax, max, lerp); 
    SmoothMin = Utility::Math::Lerp(SmoothMin, min, lerp); 
    ImGui::PlotLines("Graph", Histogram.data(), static_cast<int>(Histogram.size()), 0, 0, SmoothMin, SmoothMax, ImVec2(0, 80));
    ImGui::Text(("Max: " + std::to_string(max) + " Min: " + std::to_string(min) + " Avg: " + std::to_string(avg)).c_str());  
    ImGui::Text(("Total duration (ms): " + std::to_string(PreviousFrameTime * 1000.0)).c_str());

    if (ImGui::Button(Paused ? "Unpause" : "Pause"))
        Paused = !Paused;
    
    if (ImGui::BeginTable("Scopes", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody))
    {
        ImGui::TableSetupColumn("D", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Function");
        ImGui::TableSetupColumn("C", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("%", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("ms", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        if (CacheRoot != 0)
            DrawEntry(CacheRoot);
        
        ImGui::EndTable(); 
    }
}

void Profiling::Manager::UpdateCachedList()
{
    CachedList.clear();

    // Sorted insert
    Vector<uint32> sorted;
    for (auto& entry : PreviousScopes)
        sorted.insert( 
           std::ranges::upper_bound(sorted, entry.first, [&](const uint32 InFirst, const uint32 InSecond)
           {
               const auto& first = PreviousScopes.at(InFirst);
               const auto& second = PreviousScopes.at(InSecond);
               return first.ExecutionOrder < second.ExecutionOrder;
           }),
           entry.first);

    Vector<uint32> stack;
    uint32 last = 0;

    for (auto hash : sorted)
    {
        auto& entry = PreviousScopes.at(hash);
        if (entry.Depth == 0)
        {
            CacheRoot = hash;
            last = hash;
            stack.push_back(hash);
            break;
        }
    }

    // Ordered by total time
    for (auto hash : sorted)
    {
        if (hash == CacheRoot)
            continue;
                
        auto& entry = PreviousScopes.at(hash);
        auto* stackEntry = &PreviousScopes.at(stack.back());
            
        while (entry.Depth <= stackEntry->Depth)
        {
            stack.pop_back();
            stackEntry = &PreviousScopes.at(stack.back());
        }

        if (entry.Depth > stackEntry->Depth + 1)
        {
            stack.push_back(last);
            stackEntry = &PreviousScopes.at(last); 
        }

        auto& vec = CachedList[stack.back()];
        vec.insert( 
           std::ranges::upper_bound(vec, hash, [&](const uint32 InFirst, const uint32 InSecond)
           {
               const auto& first = PreviousScopes.at(InFirst);
               const auto& second = PreviousScopes.at(InSecond);
               return first.TotalTime > second.TotalTime;
           }),
           hash);

        last = hash;
    }
}

void Profiling::Manager::DrawEntry(uint32 InHash)
{
    auto& entry = PreviousScopes.at(InHash);
            
    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    ImGui::Text(std::to_string(entry.Depth).c_str());
            
    ImGui::TableNextColumn();
    String n = entry.Name;
    for (int i = 0; i < entry.Depth; i++)
        n = " " + n;
    ImGui::Text(n.c_str());

    ImGui::TableNextColumn();
    ImGui::Text(entry.Function.c_str());

    ImGui::TableNextColumn();
    ImGui::Text(std::to_string(entry.Calls).c_str());

    ImGui::TableNextColumn();
    ImGui::Text(std::to_string((entry.TotalTime / PreviousFrameTime) * 100.0).substr(0, 4).c_str());

    ImGui::TableNextColumn();
    ImGui::Text(std::to_string(entry.TotalTime * 1000.0).substr(0, 4).c_str());

    if (CachedList.contains(InHash))
        for (auto& entry : CachedList.at(InHash))
            DrawEntry(entry);
}
