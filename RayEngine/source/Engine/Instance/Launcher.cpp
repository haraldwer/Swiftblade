#include "Launcher.h"

#include "Manager.h"
#include "Editor/BlueprintEditor.h"
#include "Editor/RoomEditor.h"
#include "Instances/GameInstance.h"
#include "Instances/MenuInstance.h"

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

void Engine::Launcher::DrawDebugPanel()
{
    auto instance = Manager::Get().Top();
    CHECK_ASSERT(!instance, "No instance");
    
    String str = "Current instance: " + instance->GetObjName();
    ImGui::Text(str.c_str());
    ImGui::Checkbox("Save on exit", &config.SaveOnExit.Get());
    if (config.SaveOnExit)
        ImGui::BeginDisabled();
    if (ImGui::Button("Set current"))
        config.Instance = instance->GetObjName();
    ImGui::SameLine();
    if (ImGui::Button("Reset"))
        config.Instance = {};
    if (config.SaveOnExit)
        ImGui::EndDisabled();
}

#define LAUNCHER_REG_INSTANCE(x, hash) \
if (x::TypeName() == hash) { Manager::Get().Push<x>(); return; }

void Engine::Launcher::Push(const String& InName)
{
    // Always push menu as root
    Manager::Get().Push<MenuInstance>();
    LAUNCHER_REG_INSTANCE(GameInstance, InName)
    LAUNCHER_REG_INSTANCE(BlueprintEditor, InName)
    LAUNCHER_REG_INSTANCE(RoomEditor, InName)
}

#undef LAUNCHER_REG_INSTANCE