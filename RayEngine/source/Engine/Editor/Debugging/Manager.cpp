#include "Manager.h"

#include "Input/Action.h"
#include "ImGui/imgui.h"

void Debug::Manager::Init()
{
    Config.LoadConfig();
}

void Debug::Manager::Deinit()
{
    Config.SaveConfig();
}

void Debug::Manager::Logic()
{
    LogicCounter++; 
}

void Debug::Manager::Frame(double InDeltaTime)
{
    for (auto w : PendingRegister)
    {
        const String name = w->DebugWindowName();
        Windows[name].push_back(w);
        WindowToName[w] = name; 
    }
    PendingRegister.clear();

    if (IsKeyPressed(KEY_F2))
        DebugEnabled = !DebugEnabled;

    CHECK_RETURN(!DebugEnabled); 
    
    // Menu bar
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("View"))
        {
            for (const auto& entry : Windows)
            {
                CHECK_CONTINUE(entry.second.empty());
                auto& w = entry.second.back();
                CHECK_CONTINUE(!w);
                const String name = w->DebugWindowName();
                const bool open = IsOpen(name); 
                if (ImGui::MenuItem((name + (open ? " X" : "")).c_str()))
                    SetOpen(name, !open); 
            }
            ImGui::EndMenu();
        }

        ImGui::Text((" | FPS: " + std::to_string(GetFPS())).c_str());

        // Calculate ticks per frame
        const double lerp = Utility::Math::Min(2.0f * static_cast<float>(InDeltaTime), 1.0f);
        TPF = Utility::Math::Lerp(TPF, static_cast<double>(LogicCounter), lerp);
        const double tps = TPF / InDeltaTime;
        LogicCounter = 0;
        
        ImGui::Text((" TPS: " + std::to_string(tps).substr(0, 3)).c_str());
        ImGui::Text((" TPF: " + std::to_string(TPF).substr(0, 3)).c_str());
        
        ImGui::EndMainMenuBar();
    }

    // Dockspace
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    // Windows
    for (const auto& entry : Windows)
    {
        CHECK_CONTINUE(entry.second.empty());
        auto& w = entry.second.back();
        CHECK_CONTINUE(!w); 
        if (IsOpen(w->DebugWindowName()))
        {
            if (ImGui::Begin(w->DebugWindowName().c_str()))
                w->DrawDebugWindow();
            ImGui::End(); 
        }
    }
}

void Debug::Manager::Register(Window* InWindow)
{
    PendingRegister.insert(InWindow);
}

void Debug::Manager::Unregister(const Window* InWindow)
{
    const String name = WindowToName[InWindow]; 
    WindowToName.erase(InWindow); 
    auto& vec = Windows[name];
    for (int i = static_cast<int>(vec.size()) - 1; i >= 0; i--)
        if (vec[i] == InWindow || !vec[i])
            vec.erase(vec.begin() + i);
    if (Windows[name].empty())
        Windows.erase(name);
}

bool Debug::Manager::IsOpen(const String& InWindow) const
{
    return Config.OpenWindows.Get().contains(InWindow);
}

void Debug::Manager::SetOpen(const String& InWindow, bool InOpen)
{
    Set<String>& set = Config.OpenWindows.Get();
    if (InOpen)
        set.insert(InWindow);
    else
        set.erase(InWindow); 
}
