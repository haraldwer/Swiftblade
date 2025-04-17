#pragma once

#include "LightConfig.h"
#include "LightPipeline.h"
#include "Pipeline/Pipeline.h"
#include "Scene/Instances/LightInstance.h"
#include "TextureTargets/SwapTarget.h"
#include "Viewport/Viewport.h"

namespace Rendering
{
    struct RenderArgs;
    
    struct LightData
    {
        LightInstance::InstanceData Data;
        SwapTarget Target;
        Vec3F SamplePos;
        Vec3F PrevSamplePos;
        double Timestamp = 0;
        bool Initialized = false;
    };
    
    class Lights
    {
        friend class Renderer;
    public:
        void Init(const LightConfig& InConfig);
        void Deinit();
        Pipeline::Stats Update(const RenderArgs& InArgs);
        Vector<const LightInstance*> GetLights(const RenderArgs& InArgs);
        const LightData& GetData(uint32 InHash);
        
    private:
        Map<uint32, LightData> Cache;
        LightConfig Config;
        Viewport Viewport;
        LightPipeline Pipeline;
    };
}
