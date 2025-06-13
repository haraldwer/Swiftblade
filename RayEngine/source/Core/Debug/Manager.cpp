#include "Manager.h"

#include "raylib.h"
#include "ImGui/imgui.h"

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

void Debug::Manager::Frame(const double InDeltaTime)
{
    PROFILE();
    
    for (auto w : pendingRegister)
    {
        const String name = w->DebugPanelName();
        windows[name].push_back(w);
        windowToName[w] = name; 
    }
    pendingRegister.clear();

    if (IsKeyPressed(KEY_F2))
        current.DebugEnabled = !current.DebugEnabled;

    CHECK_RETURN(!current.DebugEnabled); 
    
    // Menu bar
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("View"))
        {
            for (const auto& entry : windows)
            {
                CHECK_CONTINUE(entry.second.empty());
                auto& w = entry.second.back();
                CHECK_CONTINUE(!w);
                const String name = w->DebugPanelName();
                const bool open = IsOpen(name); 
                if (ImGui::MenuItem((name + (open ? " X" : "")).c_str()))
                    SetOpen(name, !open); 
            }
            ImGui::EndMenu();
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
        
        ImGui::EndMainMenuBar();
    }

    // Dockspace
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    // Windows
    for (const auto& entry : windows)
    {
        CHECK_CONTINUE(entry.second.empty());
        auto& w = entry.second.back();
        CHECK_CONTINUE(!w); 
        if (IsOpen(w->DebugPanelName()))
        {
            if (ImGui::Begin(w->DebugPanelName().c_str()))
                w->DrawDebugPanel();
            ImGui::End(); 
        }
    }
}

void Debug::Manager::Register(Panel* InWindow)
{
    pendingRegister.insert(InWindow);
}

void Debug::Manager::Unregister(const Panel* InWindow)
{
    const String name = windowToName[InWindow]; 
    windowToName.erase(InWindow); 
    auto& vec = windows[name];
    for (int i = static_cast<int>(vec.size()) - 1; i >= 0; i--)
        if (vec[i] == InWindow || !vec[i])
            vec.erase(vec.begin() + i);
    if (windows[name].empty())
        windows.erase(name);
}

bool Debug::Manager::IsOpen(const String& InWindow) const
{
    return current.OpenWindows.Get().contains(InWindow);
}

void Debug::Manager::SetOpen(const String& InWindow, const bool InOpen)
{
    Set<String>& set = current.OpenWindows.Get();
    if (InOpen)
        set.insert(InWindow);
    else
        set.erase(InWindow); 
}
