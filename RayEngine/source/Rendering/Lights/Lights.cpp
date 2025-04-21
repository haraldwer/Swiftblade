#include "Lights.h"

#include "Context/Context.h"
#include "Pipeline/Renderer.h"
#include "RayRenderUtility.h"
#include "Scene/Scene.h"
#include "Utility/Collections/SortedInsert.h"

void Rendering::Lights::Init(const LightConfig& InConfig)
{
    Config = InConfig;
    Viewport.Init(Config.Viewport);
    AtlasMap.Init(Viewport.GetResolution(), Config.MaxLights, true);
}

void Rendering::Lights::Deinit()
{
    Viewport.Deinit();
    AtlasMap.Deinit();
}

Rendering::Pipeline::Stats Rendering::Lights::Update(const RenderArgs& InArgs)
{
    CHECK_ASSERT(!InArgs.Scene, "Invalid scene");
    CHECK_ASSERT(!InArgs.Viewport, "Invalid viewport");
    
    // Only consider lights in view, ordered by distance
    Vector<const LightInstance*> frameLights = GetLights(InArgs);

    // Filter and sort by timestamp
    Vector<LightData*> timeSortedCache;
    for (auto& light : frameLights)
    {
        CHECK_ASSERT(light->ID == 0, "Invalid hash");
        LightData& cache = Cache[light->ID];

        // Compare data
        bool cmp = cache.Data == light->Data;

        // Set data
        cache.Data = light->Data;
        cache.ID = light->ID;

        // Skip?
        CHECK_CONTINUE(cmp);
        CHECK_CONTINUE(Config.UpdateFrequency < 0.0f && cache.Timestamp > 0.001f);
        CHECK_CONTINUE(InArgs.Context->Time() - cache.Timestamp < Config.UpdateFrequency)
        Utility::SortedInsert(timeSortedCache, &cache, [&](const LightData* InFirst, const LightData* InSecond)
        {
            return InFirst->Timestamp < InSecond->Timestamp;
        });
    }

    Array<QuatF, 6> directions = RaylibRenderUtility::GetCubemapRotations();
    RenderArgs args = {
        .Scene = InArgs.Scene,
        .Context = InArgs.Context,
        .Viewport = &Viewport,
        .Lumin = InArgs.Lumin,
        .Perspectives = {}
    };
    
    Pipeline::Stats stats;
    int count = 0;
    for (auto& cache : timeSortedCache) 
    {
        CHECK_CONTINUE(!cache);
        cache->Timestamp = InArgs.Context->Time();
        cache->PrevSamplePos = cache->SamplePos;
        cache->SamplePos = cache->Data.Position;
        
        for (int i = 0; i < 6; i++)
        {
            args.Perspectives.push_back({
                .TargetRect = AtlasMap.GetRect(cache->ID, i),
                .Camera = {
                    .Position = cache->Data.Position,
                    .Rotation = directions[i],
                    .FOV = 90.0f,
                    .Far = cache->Data.Range,
                    .Near = 0.1f
                }
            });
        }
        
        count++;
        if (count >= Config.MaxShadowRenders)
            break;
    }

    Viewport.BeginFrame();
    stats += Pipeline.RenderShadows(args, Config.CollectShader);

    // TODO: Clear unused lights
    
    return stats;
}

Vector<const LightInstance*> Rendering::Lights::GetLights(const RenderArgs& InArgs)
{
    CHECK_RETURN(InArgs.Perspectives.empty(), {});
    
    auto& cam = InArgs.Perspectives.at(0).Camera;
    Frustum frustum;
    frustum.ConstructFrustum(cam, Viewport.GetResolution());
    Vector<const LightInstance*> result;

    auto sortFunc = [&](const LightInstance* InFirst, const LightInstance* InSecond)
    {
        return (InFirst->Data.Position - cam.Position).LengthSqr() < (InSecond->Data.Position - cam.Position).LengthSqr();
    };

    auto checkFunc = [&](const LightInstance& InLight)
    {
        const float camDist = (InLight.Data.Position - cam.Position).LengthSqr();
        if (camDist < InLight.Data.Range * InLight.Data.Range)
            return true;
        const float cullDist = InLight.Data.Range;
        if (frustum.CheckSphere(InLight.Data.Position, cullDist))
            return true;
        return false;
    };
    
    for (auto& light : InArgs.Scene->Lights)
        if (checkFunc(light))
            Utility::SortedInsert(result, &light, sortFunc);

    auto count = Utility::Math::Min(static_cast<int>(result.size()), Config.MaxLights.Get());
    CHECK_RETURN(count <= 0, {});
    return { result.begin(), result.begin() + count }; 
}

const Rendering::LightData& Rendering::Lights::GetData(const uint32 InHash)
{
    return Cache.at(InHash);
}

