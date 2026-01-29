#include "Manager.h"

#include "ImGui/imgui.h"

#ifdef IMGUI_ENABLE

void Debug::Manager::Init()
{
    current.LoadConfig();
}

void Debug::Manager::Deinit() const
{
    current.SaveConfig();
}

void Debug::Manager::Logic()
{
    logicCounter++; 
}

void Debug::Manager::Frame(double InDeltaTime)
{
    PROFILE();
    
    for (auto w : pendingRegister)
    {
        const String name = w->PanelName();
        const int priority = w->PanelPriority();
        panels[priority][name].push_back(w);
        panelToName[w] = name;
        panelToPriority[w] = priority;
    }
    pendingRegister.clear();

    if (ImGui::IsKeyPressed(ImGuiKey_F2))
        current.DebugEnabled = !current.DebugEnabled;

    CHECK_RETURN(!current.DebugEnabled);

    // Dockspace
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    
    // Menu bar
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("View"))
        {
            for (const auto& priority : panels)
            {
                for (const auto& entry : priority.second)
                {
                    CHECK_CONTINUE(entry.second.empty());
                    const bool open = IsOpen(entry.first);
                    
                    if (ImGui::MenuItem(entry.first.c_str()))
                        SetOpen(entry.first, !open);

                    if (open)
                    {
                        ImGui::SameLine();
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 5);
                        ImGui::Bullet();
                        ImGui::NewLine();
                    }
                }
            }
            ImGui::EndMenu();
        }
        
        ImGui::Spacing();
        //if (ImGui::MenuItem("About"))
        //    ImGui::OpenPopup("About");
        
        if (ImGui::BeginPopupModal("About", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove))
        {
            String text = "Swiftblade";
            auto windowWidth = ImGui::GetWindowSize().x;
            auto textWidth   = ImGui::CalcTextSize(text.c_str()).x;
            ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
            ImGui::Text(text.c_str());
            ImGui::Spacing();
            if (ImGui::Button("Close", {-1, 0}))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

        // Calculate ticks per frame
        float lerpSpeed = 2.0f * static_cast<float>(InDeltaTime);
        fps = Utility::Math::Lerp(fps, 1.0 / InDeltaTime, lerpSpeed);
        tpf = Utility::Math::Lerp(tpf, static_cast<double>(logicCounter), lerpSpeed);
        tps = Utility::Math::Lerp(tps, tpf / InDeltaTime, lerpSpeed);
        logicCounter = 0;
        
        ImGui::Text("| FPS: %i TPS: %i TPF: %s",
            static_cast<int>(fps),
            static_cast<int>(tps),
            std::to_string(tpf).substr(0, 3).c_str());

        ImGui::SameLine();
        ImGui::EndMainMenuBar();
    }

    // Draw the panels
    for (auto& p : panels)
    {
        for (auto& name : p.second)
        {
            CHECK_CONTINUE(name.second.empty());
            auto& panel = name.second.back();
            CHECK_CONTINUE(!panel);
            if (IsOpen(name.first))
            {
                bool open = true;
                panel->PanelBegin(open);
                panel->DrawPanel();
                panel->PanelEnd();
                if (!open) SetOpen(name.first, false);
            }
        }
    }
}

void Debug::Manager::Register(Panel* InWindow)
{
    CHECK_ASSERT(!InWindow, "Invalid window ptr");
    pendingRegister.insert(InWindow);
}

void Debug::Manager::Unregister(const Panel* InWindow)
{
    CHECK_ASSERT(!InWindow, "Invalid window ptr");
    CHECK_RETURN(panels.empty())
    const String name = panelToName.at(InWindow);
    const int priority = panelToPriority.at(InWindow);

    auto& vec = panels.at(priority).at(name);
    for (int i = vec.size() - 1; i >= 0; i--)
        if (vec.at(i) == InWindow)
            vec.erase(vec.begin() + i);
    
    panelToName.erase(InWindow);
    panelToPriority.erase(InWindow);
}

bool Debug::Manager::IsOpen(const String& InWindow) const
{
    return Enabled() && current.OpenWindows.Get().contains(InWindow);
}

void Debug::Manager::SetOpen(const String& InWindow, const bool InOpen)
{
    Set<String>& set = current.OpenWindows.Get();
    if (InOpen)
        set.insert(InWindow);
    else
        set.erase(InWindow);
    current.SaveConfig();
}

#else

void Debug::Manager::Init() {}
void Debug::Manager::Deinit() const {}
void Debug::Manager::Logic() {}
void Debug::Manager::Frame(const double InDeltaTime) {}
void Debug::Manager::Register(Panel* InWindow) {}
void Debug::Manager::Unregister(const Panel* InWindow) {}
bool Debug::Manager::IsOpen(const String& InWindow) const { return false; }
void Debug::Manager::SetOpen(const String& InWindow, const bool InOpen) { }

#endif