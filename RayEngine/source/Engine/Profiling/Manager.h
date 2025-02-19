#pragma once

#include "Editor/Debugging/Window.h"
#include "Utility/Singelton.h"
#include "Utility/Time/Timer.h"

namespace Profiling
{
    class Manager : public Utility::Singelton<Manager, true>, public Debug::Window
    {
    public:
        void Frame();
        void Begin(const String& InName, const String& InFunction);
        void End();
        
        String DebugWindowName() const override { return "Profiler"; }
        void DrawDebugWindow() override;
        void UpdateCachedList();

    private:
        void DrawEntry(uint32 InHash);
        
        struct ScopeData
        {
            String Name;
            String Function;
            int Calls = 0;
            size_t Depth = 0; 
            size_t ExecutionOrder = 0;
            double TotalTime = 0.0;
            Utility::Timer Timer; 
        };
        Map<uint32, ScopeData> Scopes;
        Map<uint32, ScopeData> PreviousScopes;
        Vector<uint32> ActiveScopes; 

        Utility::Timer FrameTimer;
        double PreviousFrameTime = 0.0;
        Vector<float> Histogram;
        float SmoothMax = 0.0f; 
        float SmoothMin = 0.0f; 

        Map<uint32, Vector<uint32>> CachedList;
        uint32 CacheRoot = 0;
        
        Utility::Timer UpdateStatTimer;
        bool Paused = false;

        
        static constexpr float StatRefreshTime = 1.0f;
        static constexpr float StatHistogramDuration = 1.0f;
    };
}
