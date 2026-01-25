#pragma once
#include "Assets/AssetBrowser.h"
#include "Debug/Log.h"
#include "Debug/Manager.h"
#include "ECS/Inspector.h"
#include "ImGui/DebugPanel.h"
#include "Launcher/Launcher.h"
#include "Utility/AssetPacker.h"

namespace Editor
{
    class Manager
    {
    public:
        void Init();
        void Tick();
        void Deinit();
        
    private:
        Debug::Manager panels;
        Launcher launcher;
        Debug::Log log;
        AssetBrowser assetBrowser;
        Inspector inspector;
        Utility::AssetPacker assetPacker;
        ImGui::DebugPanel imguiPanel;
    };
}
