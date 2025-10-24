#pragma once
#include "Panel.h"

namespace Debug
{
    class Log : public Panel
    {
    public:
        void Init();


        void DrawDebugPanel() override;
        String DebugPanelName() const override { return "Console"; }
        
    private:

        void Add(const String& InLog);
        void Clear();
        void ClearFilter();
        void UpdateFilter();
        bool CheckFilter(const String& InLog) const;
        
        Vector<String> logs;
        Vector<String> displayLogs;
        String filter;
        Set<String> filterInclude; 
        Set<String> filterDiscard; 
        int filterIndex = 0;
        bool autoscroll = true;
        bool scrollToBottom = true;
        
        static constexpr int filterStep = 100;
        static constexpr int maxDisplayCount = 1000;
    };
}
