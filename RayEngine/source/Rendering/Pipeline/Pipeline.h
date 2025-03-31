#pragma once

#include "Renderer.h"

struct RenderTexture;

namespace Rendering
{
    class Context;
    class Viewport;

    class Pipeline
    {
    public:
        virtual ~Pipeline() = default;
        
        struct Stats
        {
            Map<uint64, int> MeshDrawCount;
            int DeferredDrawCount = 0;
            int DebugDrawCount = 0;
            int Renders = 0;

            void operator += (const Stats& InStats)
            {
                Renders++;
                DebugDrawCount += InStats.DebugDrawCount;
                DeferredDrawCount += InStats.DeferredDrawCount;
                for (auto& s : InStats.MeshDrawCount)
                    MeshDrawCount[s.first] += s.second;
            }
        };
        
        virtual Stats Render(const RenderArgs& InArgs);
    
    protected:
        Renderer Renderer;
    };
}
