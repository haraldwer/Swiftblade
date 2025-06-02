#pragma once

#include "LuminConfig.h"
#include "LuminPipeline.h"
#include "Rendering/Context/Context.h"
#include "Rendering/TextureTargets/AtlasMap.h"
#include "Rendering/Viewport/Viewport.h"

namespace Rendering
{
    struct RenderArgs;
    class Scene;
    
    struct ProbeCoord
    {
        union
        {
            struct
            {
                int16 x; 
                int16 y; 
                int16 z; 
                int16 layer; 
            };

            uint64 id = 0;
        };
    };
    
    struct LuminProbe
    {
        ProbeCoord coord = {};
        Vec3F pos = {};
        double timestamp = 0.0;
        int iterations = 0;
        Vec4F rect = {};
    };
    
    class Lumin
    {
        friend class Renderer;
    public:
        void Init(const LuminConfig& InConfig);
        void Deinit();

        Pipeline::Stats Update(const RenderArgs& InArgs);
        Vector<LuminProbe*> GetProbes(const RenderArgs& InArgs); // Unsafe!
        RenderTarget& GetProbeTarget() { return lerpTarget.Curr(); }
        float GetRange() const;

    private:
        Pipeline::Stats UpdateProbes(const RenderArgs& InArgs);
        Pipeline::Stats LerpProbes(const RenderArgs& InArgs);
        void ExpandVolume(const Scene& InScene);
        void TryCreateProbe(ProbeCoord InCoord);
        ProbeCoord FromPos(const Vec3F& InPos);
        Vec3F FromCoord(const ProbeCoord& InCoord);

        Map<uint64, LuminProbe> probes = {};

        LuminConfig config = {};
        Context context = {};
        Viewport viewport = {};
        RenderTarget target = {};
        SwapTarget lerpTarget = {};
        AtlasMap atlasMap = {};
        LuminPipeline pipeline = {};
    };
}

