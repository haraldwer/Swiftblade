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
        LightInstance::InstanceData data = {};
        Vec3F pos = {};
        double timestamp = 0;
        uint32 id = static_cast<uint32>(-1);
        Vec4F rect = {};
    };
    
    class Lights
    {
        friend class LightsRenderer;
    public:
        void Init(const LightConfig& InConfig);
        void Deinit();
        Pipeline::Stats Update(const RenderArgs& InArgs);
        Vector<const LightInstance*> GetLights(const RenderArgs& InArgs);
        RenderTarget& GetShadowTarget() { return target; }
        const LightData& GetData(uint32 InHash);

        Vec2F GetFaceTexel() const;

    private:
        Map<uint32, LightData> cache = {};
        
        LightConfig config = {};
        Viewport atlasView = {};
        AtlasMap atlas = {};
        RenderTarget target = {};
    };
}
