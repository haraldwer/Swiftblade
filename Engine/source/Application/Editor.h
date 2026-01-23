#pragma once
#include "Assets/AssetBrowser.h"
#include "Debug/Log.h"
#include "Debug/Manager.h"
#include "ImGui/DebugPanel.h"
#include "Utility/AssetPacker.h"

namespace Application
{
    class Editor
    {
    public:
        void Init();
        void Tick();
        void Deinit();
        
    private:
        Debug::Manager panels;
        Debug::Log log;
        ::Editor::AssetBrowser assetBrowser;
        Utility::AssetPacker assetPacker;
        ImGui::DebugPanel imguiPanel;
    };
}
