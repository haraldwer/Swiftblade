#pragma once

#include "Renderer.h"

namespace Rendering
{
    class DeferredRenderer : public Renderer
    {
    public:
        static int DrawSkyboxes(const RenderArgs& InArgs, const RenderTarget& InTarget);
        static Map<uint64, int> DrawScene(const RenderArgs& InArgs, RenderTarget& InSceneTarget);
        static int DrawDeferredScene(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers);
        static int DrawSurfaces(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers);
    };
}
