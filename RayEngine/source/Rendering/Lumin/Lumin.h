#pragma once

#include "Context/Context.h"
#include "LuminPipeline.h"
#include "Viewport/Viewport.h"

namespace Rendering
{
    struct RenderArgs;
    class Scene;

    struct LuminConfig : BaseConfig<LuminConfig>
    {
        PROPERTY(ContextConfig, Context);
        PROPERTY(ViewportConfig, Viewport);
        PROPERTY_D(ResShader, BlipShader, "Shaders/Lumin/SH_LuminProbe.ps");
        PROPERTY_D(float, Density, 1.0f);

        String Name() const override { return "Lumin"; }
    };
    
    class Lumin
    {
        struct Coord
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
        
        struct Probe
        {
            RenderTarget Target;
            Coord Coord;
            Vec3F Pos;
        };

    public:
        void Init(const ContextConfig& InDefaultContextConfig);
        void Deinit();
        
        void UpdateProbes(const RenderArgs& InArgs);

    private:
        void ExpandVolume(const Scene& InScene, const RenderTexture& InTex);
        Coord FromPos(const Vec3F& InPos);
        Vec3F FromCoord(const Coord& InCoord);

        Map<uint64, Probe> Probes;

        LuminConfig Config;
        Context Context;
        Viewport Viewport;
        LuminPipeline Pipeline;
    };
}

