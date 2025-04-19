#pragma once

#include "Context/Context.h"
#include "LuminConfig.h"
#include "LuminPipeline.h"
#include "Viewport/Viewport.h"

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
        ProbeCoord Coord;
        Vec3F Pos;
        RenderTarget Target;
        double Timestamp = 0.0;
        int Iterations = 0;
        // TODO: Fadein/out
    };
    
    class Lumin
    {
    public:
        void Init(const LuminConfig& InConfig);
        void Deinit();

        LuminConfig GetConfig() const { return Config; }
        Vector<LuminProbe*> GetProbes(const RenderArgs& InArgs); // Unsafe!
        Pipeline::Stats Update(const RenderArgs& InArgs);

    private:
        void ExpandVolume(const Scene& InScene);
        void TryCreateProbe(ProbeCoord InCoord);
        ProbeCoord FromPos(const Vec3F& InPos);
        Vec3F FromCoord(const ProbeCoord& InCoord);

        Map<uint64, LuminProbe> Probes;

        LuminConfig Config;
        Context Context;
        Viewport Viewport;
        LuminPipeline Pipeline;
    };
}

