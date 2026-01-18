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
        Vec3F probeSize;
        Vec3F gridSize;
    };
    
    class Lumin
    {
        friend class LuminRenderer;
    public:
        void Init(const ContextConfig& InConfig);
        void Deinit();

        Pipeline::Stats Update(const RenderArgs& InArgs);
        
        // Probe data
        LuminData GetFrameData();
        Vector<Mat4F> GetDebugProbes(const RenderArgs& InArgs);
    
    private:
        Vec3F SelectProbe(const RenderArgs& InArgs);
        Array<Vec3F, 9> GetCullPoints(const Vec3F &InPos) const;
        Pipeline::Stats UpdateProbes(const RenderArgs& InArgs);
        Pipeline::Stats LerpProbes(const RenderArgs& InArgs);
        
        LuminConfig config = {};
        Context context = {};
        Viewport viewport = {};
        
        Vec3I frustumMin;
        Vec3I frustumMax;
        
        
    };
}