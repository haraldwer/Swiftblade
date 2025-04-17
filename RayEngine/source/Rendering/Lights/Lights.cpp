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
}

void Rendering::Lights::Deinit()
{
    for (auto& l : Cache)
        for (auto& t : l.second.Target.All())
            t.Unload();
    Viewport.Deinit();
}

Rendering::Pipeline::Stats Rendering::Lights::Update(const RenderArgs& InArgs)
{
    CHECK_ASSERT(!InArgs.Scene, "Invalid scene");
    CHECK_ASSERT(!InArgs.Viewport, "Invalid viewport");

    // Create all textures instantly
    for (auto& light : InArgs.Scene->Lights)
    {
        auto& cache = Cache[light.ID];
        CHECK_CONTINUE(cache.Initialized);
        for (auto& target : cache.Target.All())
        {
            auto& tex = Viewport.GetVirtualTarget();
            if (target.TryBeginSetup(tex))
            {
                target.CreateBuffer(
                    "TexShadowCube",
                    PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
                    1.0f,
                    1,
                    true);
                target.EndSetup(tex);
            }
        }
    }
    
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

        // Skip?
        CHECK_CONTINUE(cmp);
        CHECK_CONTINUE(Config.UpdateFrequency < 0.0f && cache.Timestamp > 0.001f);
        CHECK_CONTINUE(InArgs.Context->Time() - cache.Timestamp < Config.UpdateFrequency)
        Utility::SortedInsert(timeSortedCache, &cache, [&](const LightData* InFirst, const LightData* InSecond)
        {
            return InFirst->Timestamp < InSecond->Timestamp;
        });
    }

    auto directions = RaylibRenderUtility::GetCubemapRotations();
    RenderArgs args = {
        .Scene = InArgs.Scene,
        .Context = InArgs.Context,
        .Viewport = &Viewport,
        .Lumin = InArgs.Lumin,
        .Camera = CameraInstance()
    };

    Viewport.BeginFrame();
    Pipeline::Stats stats;
    int count = 0;
    for (auto& cache : timeSortedCache) 
    {
        CHECK_CONTINUE(!cache);
        cache->Target.Iterate();
        cache->Timestamp = InArgs.Context->Time();
        cache->PrevSamplePos = cache->SamplePos;
        cache->SamplePos = cache->Data.Position;
        for (int i = 0; i < 6; i++)
        {
            args.Camera = CameraInstance {
                .Position = cache->Data.Position,
                .Rotation = directions[i],
                .FOV = 90.0f,
                .Far = cache->Data.Range,
                .Near = 0.1f
            };
            stats += Pipeline.RenderShadowFace(args, cache->Target.Curr(), Config.CollectShader, i);
        }
        count++;
        if (count >= Config.MaxShadowRenders)
            break;
    }
    Viewport.EndFrame();
    BeginTextureMode(InArgs.Viewport->GetVirtualTarget());

    // TODO: Clear unused lights
    
    return stats;
}

Vector<const LightInstance*> Rendering::Lights::GetLights(const RenderArgs& InArgs)
{
    Frustum frustum;
    frustum.ConstructFrustum(InArgs.Camera);
    Vector<const LightInstance*> result;

    auto sortFunc = [&](const LightInstance* InFirst, const LightInstance* InSecond)
    {
        return (InFirst->Data.Position - InArgs.Camera.Position).LengthSqr() < (InSecond->Data.Position - InArgs.Camera.Position).LengthSqr();
    };

    auto checkFunc = [&](const LightInstance& InLight)
    {
        const float camDist = (InLight.Data.Position - InArgs.Camera.Position).LengthSqr();
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

