#include "Manager.h"

#include "ImGui/imgui.h"

void Debug::Manager::Init()
{
    Config.LoadConfig();
}

void Debug::Manager::Deinit()
{
    Config.SaveConfig();
}

void Debug::Manager::DrawDebugWindow()
{
    for (auto w : PendingRegister)
    {
        const String name = w->DebugWindowName();
        CHECK_ASSERT(NameToWindow.contains(name), "Window already registered"); 
        NameToWindow[name] = w;
        WindowToName[w] = name; 
        if (Config.OpenWindows.Get().contains(name))
            w->ToggleDebugWindow();
    }
    PendingRegister.clear();

    if (IsKeyPressed(KEY_F2))
        DebugEnabled = !DebugEnabled;

    CHECK_RETURN(!DebugEnabled); 

    // Update open state
    auto& openWindows = Config.OpenWindows.Get();
    for (const auto& w : NameToWindow)
    {
        if (w.second->IsDebugWindowOpen())
            openWindows.insert(w.first);
        else
            openWindows.erase(w.first);
    }

    // Menu bar
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("View"))
        {
            for (const auto w : NameToWindow)
            {
                String item = w.second->DebugWindowName();
                if (w.second->IsDebugWindowOpen())
                    item += " X";
                if (ImGui::MenuItem(item.c_str()))
                    w.second->ToggleDebugWindow(); 
            }
            ImGui::EndMenu();
        }

        ImGui::Text((" | FPS: " + std::to_string(GetFPS())).c_str());
        ImGui::EndMainMenuBar();
    }

    // Dockspace
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    // Windows
    for (const auto w : NameToWindow)
    {
        if (w.second->IsDebugWindowOpen())
        {
            if (ImGui::Begin(w.second->DebugWindowName().c_str()))
                w.second->DrawDebugWindow();
            ImGui::End(); 
        }
    }
}

void Debug::Manager::Register(Window* InWindow)
{
    CHECK_ASSERT(WindowToName.contains(InWindow), "Window already registered"); 
    PendingRegister.insert(InWindow);
}

void Debug::Manager::Unregister(Window* InWindow)
{
    const String name = WindowToName[InWindow];
    WindowToName.erase(InWindow); 
    NameToWindow.erase(name); 
}
