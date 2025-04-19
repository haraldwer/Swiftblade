#pragma once

#include "Renderer.h"

struct RenderTexture;

namespace Rendering
{
    struct FrameTargetCollection;
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
            int FullscreenPasses = 0;
            int Probes = 0;
            int Lights = 0;
            int Skyboxes = 0;

            void operator += (const Stats& InStats)
            {
                Renders++;
                DebugDrawCount += InStats.DebugDrawCount;
                DeferredDrawCount += InStats.DeferredDrawCount;
                FullscreenPasses += InStats.FullscreenPasses;
                for (auto& s : InStats.MeshDrawCount)
                    MeshDrawCount[s.first] += s.second;
            }
        };
        
        virtual Stats Render(RenderArgs InArgs);

    protected:
        Stats RenderSkybox(const RenderArgs& InArgs);
        Stats RenderScene(const RenderArgs& InArgs);
        Stats RenderDeferred(const RenderArgs& InArgs);
        Stats RenderLights(const RenderArgs& InArgs);
        Stats RenderLumin(const RenderArgs& InArgs);
        Stats RenderFire(const RenderArgs& InArgs);
        Stats ApplyFire(const RenderArgs& InArgs);
        Stats RenderAO(const RenderArgs& InArgs);
        Stats RenderFX(const RenderArgs& InArgs);
        Stats Blip(const RenderArgs& InArgs);
        Stats RenderDebug(const RenderArgs& InArgs);
    };
}
