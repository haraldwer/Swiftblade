#pragma once

#include "LuminConfig.h"
#include "LuminPipeline.h"
#include "Collections/PersistenceContainer.h"
#include "Math/Coord.h"
#include "Rendering/Context/Context.h"
#include "Rendering/TextureTargets/AtlasMap.h"
#include "Rendering/Viewport/Viewport.h"

namespace Rendering
{
    struct RenderArgs;
    class Scene;

    typedef Utility::Coord<uint64, int16> ProbeCoord;
    
    struct LuminProbe
    {
        ProbeCoord coord = {};
        Vec3F pos = {};
        double atlasTimestamp = 0.0;
        double renderTimestamp = 0.0;
        int iterations = 0;
        Vec4F rect = {};
    };

    struct LuminRenderData
    {
        struct Layer
        {
            Vec3I start = Vec3I::Zero();
            Vec3I size = Vec3I::Zero();
            Vec3F density = Vec3F::One();
            int startIndex = 0;
            int endIndex = 0;
        };
        LuminProbe* fallback;
        Vector<Layer> layers = {};
        Vector<int> indices = {}; // Only some slots will be filled
        Vector<LuminProbe*> probes;
        Vector<float> timestamps;
    };
    
    class Lumin
    {
        friend class LuminRenderer;
    public:
        void Init(const ContextConfig& InConfig);
        void Deinit();

        Pipeline::Stats Update(const RenderArgs& InArgs);
        LuminRenderData GetFrameProbes(const RenderArgs& InArgs);
        RenderTarget& GetProbeTarget() { return lerpTarget.Curr(); }
        RenderTarget& GetFrameTarget() { return target; }

    private:
        Vector<LuminProbe*> GetProbes(const RenderArgs& InArgs, int InLayer); // Unsafe!
        Pipeline::Stats UpdateProbes(const RenderArgs& InArgs);
        Pipeline::Stats UpdateFallbackProbe(const RenderArgs& InArgs);
        Pipeline::Stats LerpProbes(const RenderArgs& InArgs);
        void ExpandVolume(const RenderArgs& InArgs);
        void CreateProbe(ProbeCoord InCoord);
        void RemoveProbe(ProbeCoord InCoord);
        ProbeCoord FromPos(const Vec3F& InPos, int InLayer) const;
        Vec3F FromCoord(const ProbeCoord& InCoord) const;
        Vec3F GetDensity(int InLayer) const;

        Array<Vec3F, 9> GetCullPoints(const Vec3F &InPos) const;

        LuminConfig config = {};
        Context context = {};
        Viewport viewport = {};
        LuminPipeline pipeline = {};

        // Probe data
        LuminProbe fallback;
        Map<uint64, LuminProbe> probes = {};
        Map<int, Set<uint64>> layerProbes = {};
        AtlasMap atlas = {};
        RenderTarget target = {};
        SwapTarget lerpTarget = {};

        Utility::PersistenceContainer<uint64> probePersistence = {};

        struct RenderPersistence
        {
            Utility::PersistenceContainer<uint64> persistence;
            Map<uint64, float> times;
        };
        Map<uint8, RenderPersistence> renderPersistence = {};
    };
}

