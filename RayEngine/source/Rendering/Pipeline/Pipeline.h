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
            Map<uint64, int> meshDrawCount = {};
            int deferredDrawCount = 0;
            int debugDrawCount = 0;
            int renders = 0;
            int fullscreenPasses = 0;
            int probes = 0;
            int lights = 0;
            int skyboxes = 0;

            void operator += (const Stats& InStats)
            {
                renders++;
                debugDrawCount += InStats.debugDrawCount;
                deferredDrawCount += InStats.deferredDrawCount;
                fullscreenPasses += InStats.fullscreenPasses;
                for (auto& s : InStats.meshDrawCount)
                    meshDrawCount[s.first] += s.second;
            }
        };


        virtual Stats Render(RenderArgs InArgs);

    protected:
        static Stats RenderSkybox(const RenderArgs& InArgs);
        static Stats RenderScene(const RenderArgs& InArgs);
        static Stats ProcessScene(const RenderArgs& InArgs);
        static Stats RenderDeferred(const RenderArgs& InArgs);
        static Stats RenderLights(const RenderArgs& InArgs);
        static Stats RenderLumin(const RenderArgs& InArgs);
        static Stats RenderAO(const RenderArgs& InArgs);
        static Stats RenderFX(const RenderArgs& InArgs);
        static Stats Blip(const RenderArgs& InArgs);
        static Stats RenderDebug(const RenderArgs& InArgs);
    };
}
