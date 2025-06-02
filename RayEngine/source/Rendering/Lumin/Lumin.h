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
        ProbeCoord Coord = {};
        Vec3F Pos = {};
        double Timestamp = 0.0;
        int Iterations = 0;
        Vec4F Rect = {};
    };
    
    class Lumin
    {
        friend class Renderer;
    public:
        void Init(const LuminConfig& InConfig);
        void Deinit();

        Pipeline::Stats Update(const RenderArgs& InArgs);
        Vector<LuminProbe*> GetProbes(const RenderArgs& InArgs); // Unsafe!
        RenderTarget& GetProbeTarget() { return LerpTarget.Curr(); }
        float GetRange() const;

    private:
        Pipeline::Stats UpdateProbes(const RenderArgs& InArgs);
        Pipeline::Stats LerpProbes(const RenderArgs& InArgs);
        void ExpandVolume(const Scene& InScene);
        void TryCreateProbe(ProbeCoord InCoord);
        ProbeCoord FromPos(const Vec3F& InPos);
        Vec3F FromCoord(const ProbeCoord& InCoord);

        Map<uint64, LuminProbe> Probes = {};

        LuminConfig Config = {};
        Context Context = {};
        Viewport Viewport = {};
        RenderTarget Target = {};
        SwapTarget LerpTarget = {};
        AtlasMap AtlasMap = {};
        LuminPipeline Pipeline = {};
    };
}

