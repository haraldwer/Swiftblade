#include "Log.h"

#ifdef IMGUI_ENABLE

void Debug::Log::Init()
{
    Utility::AddLogCallback([&](const String& InMessage) { Add(InMessage); });
    Clear();
}

void Debug::Log::DrawPanel()
{
    if (ImGui::ArrowButton("Bottom", ImGuiDir_Down))
        scrollToBottom = true;
    ImGui::SameLine();
    if (ImGui::Button("Clear"))
        Clear();
    ImGui::SameLine();
    ImGui::Text(" Auto: ");
    ImGui::SameLine();
    ImGui::Checkbox("##Auto", &autoscroll);
    ImGui::SameLine();
    ImGui::Text(" Filter: ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(-1);
    if (ImGui::InputText("##Filter", &filter))
        ClearFilter();
    
    UpdateFilter();
    if (ImGui::BeginChild("LogRegion", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
    {
        for (auto& log : displayLogs)
        {
            // TODO: Colors
            ImVec4 color = ImVec4(1, 1, 1, 1);
            if (log.contains("ERROR |"))
                color = ImVec4(1, 0.5, 0.5, 1);
            else if (log.contains("WARN |"))
                color = ImVec4(1, 1, 0.5, 1);
            else if (log.contains("INFO |"))
                color = ImVec4(0.5, 0.5, 0.5, 1);
            if (color != ImVec4(1, 1, 1, 1))
                ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(log.c_str());
            if (color != ImVec4(1, 1, 1, 1))
                ImGui::PopStyleColor();
        }

        // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
        // Using a scrollbar or mouse-wheel will take away from the bottom edge.
        if (scrollToBottom || (autoscroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        scrollToBottom = false;
    }
    ImGui::EndChild();
}

void Debug::Log::Add(const String &InLog)
{
    logs.push_back(InLog);
    filterIndex++;
    if (CheckFilter(InLog))
        displayLogs.push_back(InLog);
}

void Debug::Log::Clear()
{
    logs.clear();
    ClearFilter();
}

void Debug::Log::ClearFilter()
{
    displayLogs.clear();
    filterIndex = 0;
    filterInclude.clear();
    filterDiscard.clear();

    String quote;
    bool quoted = false;
    
    Utility::StringSplit(filter, " ", [&](const String& InStr)
    {
        if (!quoted)
        {
            if (InStr.starts_with('\"'))
            {
                quoted = true;
                quote += InStr.substr(1);
                return;
            }
        }
        else
        {
            if (InStr.ends_with('\"'))
            {
                quote += InStr.substr(0, InStr.size() - 1);
                filterInclude.insert(quote);
                quoted = false;
                quote = "";
            }
            else
            {
                quote += " " + InStr;
            }
            return;
        }
        
        if (InStr.starts_with('-'))
            filterDiscard.insert(InStr.substr(1));
        else if (InStr.starts_with('+'))
            filterInclude.insert(InStr.substr(1));
        else
            filterInclude.insert(InStr);
    });

    if (quoted)
        filterInclude.insert(quote);
}

void Debug::Log::UpdateFilter()
{
    // Filter index moves along logs and adds to displayLogs
    // logs and displaylogs should be the same?
    displayLogs.reserve(logs.size());
    int startIndex = filterIndex;
    for (int i = startIndex; i < startIndex + filterStep && i < static_cast<int>(logs.size()) && static_cast<int>(displayLogs.size()) < maxDisplayCount; i++)
    {
        filterIndex++;
        auto& log = logs.at(logs.size() - i - 1); // reverse 
        if (CheckFilter(log))
            displayLogs.insert(displayLogs.begin(), log);
    }
}

bool Debug::Log::CheckFilter(const String &InLog) const
{
    if (filter.empty())
        return true;
    for (auto& discard : filterDiscard)
        if (InLog.contains(discard))
            return false;
    for (auto& include : filterInclude)
        if (!InLog.contains(include))
            return false;
    return true;
}

#else

void Debug::Log::Init() {}
void Debug::Log::DrawDebugPanel() {}
void Debug::Log::Add(const String &InLog) {}
void Debug::Log::Clear() {}
void Debug::Log::ClearFilter() {}
void Debug::Log::UpdateFilter() {}
bool Debug::Log::CheckFilter(const String &InLog) const { return false; }

#endif