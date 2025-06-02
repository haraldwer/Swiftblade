#pragma once

#include "LightConfig.h"
#include "LightPipeline.h"
#include "Rendering/Pipeline/Pipeline.h"
#include "Rendering/Scene/Instances/LightInstance.h"
#include "Rendering/TextureTargets/AtlasMap.h"
#include "Rendering/TextureTargets/RenderTarget.h"
#include "Rendering/Viewport/Viewport.h"

namespace Rendering
{
    struct RenderArgs;
    
    struct LightData
    {
        LightInstance::InstanceData Data = {};
        Vec3F Pos = {};
        double Timestamp = 0;
        uint32 ID = static_cast<uint32>(-1);
        Vec4F Rect = {};
    };
    
    class Lights
    {
        friend class Renderer;
    public:
        void Init(const LightConfig& InConfig);
        void Deinit();
        Pipeline::Stats Update(const RenderArgs& InArgs);
        Vector<const LightInstance*> GetLights(const RenderArgs& InArgs);
        RenderTarget& GetShadowTarget() { return Target; }
        const LightData& GetData(uint32 InHash);
        
    private:
        Map<uint32, LightData> Cache = {};
        
        LightConfig Config = {};
        Viewport AtlasView = {};
        AtlasMap Atlas = {};
        RenderTarget Target = {};
        LightPipeline Pipeline = {};
    };
}
