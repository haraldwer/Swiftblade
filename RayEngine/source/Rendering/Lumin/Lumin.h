#pragma once

#include "LuminChunk.h"
#include "LuminConfig.h"
#include "LuminPipeline.h"
#include "Rendering/Context/Context.h"
#include "Rendering/Viewport/Viewport.h"

namespace Rendering
{
    struct RenderArgs;
    class Scene;
    
    struct LuminData
    {
        Vector<LuminChunkFrameData> visibleChunks;
        Vec3F cellSize;
        Vec3F chunkSize;
    };
    
    class Lumin
    {
        friend class LuminRenderer;
    public:
        void Init(const ContextConfig& InConfig);
        void Deinit();

        Pipeline::Stats Update(const RenderArgs& InArgs);
        LuminData GetFrameData(const RenderArgs& InArgs);
        Vector<Mat4F> GetDebugProbes();

    private:
        Pipeline::Stats UpdateProbes(const RenderArgs& InArgs);
        
        Array<Vec3F, 9> GetCullPoints(const Vec3F &InPos) const;
        static void GetFrustum(const RenderArgs& InArgs, Frustum& OutFrustum, Vec3F& OutMin, Vec3F& OutMax);
        RenderArgs GetCubemapArgs(const RenderArgs& InArgs, const Vec3F& InProbePos);
        
        LuminConfig config = {};
        Context context = {};
        Viewport viewport = {};
        LuminChunkCollection chunks;
        RenderTarget frame;
        Vector<Mat4F> debugProbes;
    };
}