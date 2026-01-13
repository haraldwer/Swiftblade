#include "Launcher.h"

#include "Manager.h"
#include "Editor/BlueprintEditor.h"
#include "Game/Editor/RoomEditor.h"
#include "Game/Instances/MenuInstance.h"
#include "Game/Instances/GameInstance.h"
#include "Game/Instances/RenderInstance.h"

#ifdef IMGUI_ENABLE

void Engine::Launcher::Init()
{
    config.LoadConfig();
    Push(config.Instance);
}

void Engine::Launcher::Deinit()
{
    if (config.SaveOnExit)
        if (auto instance = Manager::Get().Top())
            config.Instance = instance->GetObjName();
    config.SaveConfig();
}

void Engine::Launcher::DrawPanel()
{
    auto instance = Manager::Get().Top();
    CHECK_ASSERT(!instance, "No instance");
    
    String str = "Current instance: " + instance->GetObjName();
    ImGui::Text(str.c_str());
    ImGui::Checkbox("Save on exit", &config.SaveOnExit.Get());
    if (config.SaveOnExit)
        ImGui::BeginDisabled();
    if (ImGui::Button("Set current"))
    {
        config.Instance = instance->GetObjName();
        config.SaveConfig();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset"))
    {
        config.Instance = {};
        config.SaveConfig();
    }
    if (config.SaveOnExit)
        ImGui::EndDisabled();
}

#define LAUNCHER_REG_INSTANCE(x, hash) \
if (x::TypeName() == hash) { Manager::Get().Push<x>(); return; }

void Engine::Launcher::Push(const String& InName)
{
    // Always push menu as root
    Manager::Get().Push<MenuInstance>();

#ifndef __EMSCRIPTEN__
    LAUNCHER_REG_INSTANCE(GameInstance, InName)
    LAUNCHER_REG_INSTANCE(BlueprintEditor, InName)
    LAUNCHER_REG_INSTANCE(RoomEditor, InName)
    LAUNCHER_REG_INSTANCE(RenderInstance, InName)
#endif
}

#undef LAUNCHER_REG_INSTANCE

#else

void Engine::Launcher::Init()
{
    Manager::Get().Push<MenuInstance>();
}

void Engine::Launcher::Deinit() {}
void Engine::Launcher::DrawDebugPanel() {}
void Engine::Launcher::Push(const String& InName) {}

#endif