#pragma once

#include "LuminConfig.h"
#include "LuminPipeline.h"
#include "Collections/GridSplitContainer.h"
#include "Math/Coord.h"
#include "Rendering/Context/Context.h"
#include "Rendering/TextureTargets/AtlasMap.h"
#include "Rendering/Viewport/Viewport.h"

namespace Rendering
{
    struct RenderArgs;
    class Scene;
    using LuminCoord = Utility::Coord<uint64, uint16>;  

    struct LuminData
    {
        RenderTarget* shTarget;
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
        
        // The last rendered probe (for debugging)
        RenderTarget& GetFrameTarget() { return target; }
    
    private:
        Vec3F SelectProbe(const RenderArgs& InArgs);
        Array<Vec3F, 9> GetCullPoints(const Vec3F &InPos) const;
        Pipeline::Stats UpdateProbes(const RenderArgs& InArgs);
        Pipeline::Stats LerpProbes(const RenderArgs& InArgs);
        
        LuminConfig config = {};
        Context context = {};
        Viewport viewport = {};
        
        // For calculating ordering etc
        using TimeGrid = Utility::GridSplitContainer<float>; 
        TimeGrid timestamps;
        Vec3I frustumMin;
        Vec3I frustumMax;
        bool fallbackRendered = false;
        
        // Each entry is just 9 rgba pixels in a row
        RenderTarget target = {};
        SwapTarget lerpTarget = {};
    };
}