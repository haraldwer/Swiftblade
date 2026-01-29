#include "Launcher.h"

#include "Instance/InstanceManager.h"
#include "ECS/BlueprintEditor.h"
#include "Editor/RoomEditor.h"
#include "Instances/MenuInstance.h"
#include "Instances/GameInstance.h"
#include "Instances/RenderInstance.h"

void Editor::Launcher::Init()
{
    config.LoadConfig();
    //Push(config.Instance);
}

void Editor::Launcher::Deinit()
{
    if (config.SaveOnExit)
        if (auto instance = Engine::InstanceManager::Get().Top())
            config.Instance = instance->GetObjName();
    config.SaveConfig();
}

void Editor::Launcher::DrawPanel()
{
    auto instance = Engine::InstanceManager::Get().Top();
    if (!instance)
    {
        ImGui::Text("No current instance");
        return;
    }
    
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
if (x::TypeName() == hash) { Engine::InstanceManager::Get().Push<x>(); return; }

void Editor::Launcher::Push(const String& InName)
{
    // Always push menu as root
    Engine::InstanceManager::Get().Push<MenuInstance>();

#ifndef __EMSCRIPTEN__
    LAUNCHER_REG_INSTANCE(GameInstance, InName)
    LAUNCHER_REG_INSTANCE(BlueprintEditor, InName)
    LAUNCHER_REG_INSTANCE(RoomEditor, InName)
    LAUNCHER_REG_INSTANCE(RenderInstance, InName)
#endif
}

#undef LAUNCHER_REG_INSTANCE
