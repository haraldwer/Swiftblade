#pragma once
#include "Context.h"
#include "Pipeline/Pipeline.h"
#include "Utility/pch.h"
#include "Viewport/Viewport.h"

class RenderTarget;
struct TextureCubemap;

namespace Rendering
{
    struct RenderArgs;
    class Scene;

    struct LuminConfig : BaseConfig<LuminConfig>
    {
        PROPERTY(ContextConfig, Context);
        PROPERTY(ViewportConfig, Viewport);
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
            TextureCubemap* Tex = nullptr;
            Coord Coord;
            Vec3F Pos;
        };

    public:
        
        void Init();
        void UpdateProbes(const Scene& InScene);
        void Render(const RenderArgs& InArgs, const RenderTarget& InTarget);
        
    private:
        void ExpandVolume(const Scene& InScene);
        Coord FromPos(const Vec3F& InPos);

        Map<uint64, Probe> Probes;

        LuminConfig Config;
        Context Context;
        Viewport Viewport;
        Pipeline Pipeline;
    };
}

