#pragma once

#include "Context/Context.h"
#include "LuminPipeline.h"
#include "raylib.h"
#include "Resources/Model.h"
#include "Viewport/Viewport.h"

namespace Rendering
{
    struct RenderArgs;
    class Scene;

    struct LuminConfig : BaseConfig<LuminConfig>
    {
        PROPERTY(ContextConfig, Context);
        PROPERTY(ViewportConfig, Viewport);
        PROPERTY_D(ResShader, LightingShader, "Shaders/Lumin/SH_Lumin.ps");
        PROPERTY_D(ResShader, DebugShader, "Shaders/Lumin/SH_LuminDebug.fs");
        PROPERTY_D(ResShader, ProbeShader, "Shaders/Lumin/SH_LuminProbe.ps");
        PROPERTY_D(ResModel, SphereModel, "Defaults/M_Sphere.obj");
        
        PROPERTY_D(Vec3F, Density, Vec3F(0.75f, 1.0f, 0.75f) * 0.1f);
        PROPERTY_D(float, CloseCullDistance, 10.0f);
        PROPERTY_D(float, UpdateFrequency, 1.0f);
        PROPERTY_D(int, Iterations, 0);
        PROPERTY_D(int, MaxProbeRenders, 1);
        PROPERTY_D(float, ProbeFar, 50.0f);
        PROPERTY_D(int, MaxProbes, 30);
        
        PROPERTY_D(bool, Debug, false);

        String Name() const override { return "Lumin"; }
    };

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
        void Init(const ContextConfig& InDefaultContextConfig);
        void Deinit();

        LuminConfig GetConfig() const { return Config; }
        Vector<LuminProbe*> GetProbes(const RenderArgs& InArgs); // Unsafe!
        Pipeline::Stats UpdateProbes(const RenderArgs& InArgs);

    private:
        void ExpandVolume(const Scene& InScene);
        void TryCreateProbe(const RenderTexture& InTex, ProbeCoord InCoord);
        ProbeCoord FromPos(const Vec3F& InPos);
        Vec3F FromCoord(const ProbeCoord& InCoord);

        Map<uint64, LuminProbe> Probes;

        LuminConfig Config;
        Context Context;
        Viewport Viewport;
        LuminPipeline Pipeline;
    };
}

