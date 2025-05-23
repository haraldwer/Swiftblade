#include "Lights.h"

#include "Context/Context.h"
#include "Pipeline/Renderer.h"
#include "RayRenderUtility.h"
#include "Scene/Scene.h"
#include "Utility/Collections/SortedInsert.h"

void Rendering::Lights::Init(const LightConfig& InConfig)
{
    Config = InConfig;
    AtlasView.Init(Config.Viewport);
    Atlas.Init(AtlasView.GetResolution(), Config.MaxLights, true);
    Target.Setup(AtlasView.GetVirtualTarget(), "TexShadow", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
}

void Rendering::Lights::Deinit()
{
    AtlasView.Deinit();
    Atlas.Deinit();
    Cache.clear();
    Target.Unload();
}

Rendering::Pipeline::Stats Rendering::Lights::Update(const RenderArgs& InArgs)
{
    PROFILE_GL();
    
    CHECK_ASSERT(!InArgs.ScenePtr, "Invalid scene");
    CHECK_ASSERT(!InArgs.ViewportPtr, "Invalid viewport");
    
    // Only consider lights in view, ordered by distance
    Vector<const LightInstance*> frameLights = GetLights(InArgs);

    // Filter and sort by timestamp
    Vector<LightData*> timeSortedCache;
    for (auto& light : frameLights)
    {
        CHECK_ASSERT(light->ID == 0, "Invalid hash");
        LightData& cache = Cache[light->ID];

        // Set data
        cache.Data = light->Data;
        cache.ID = light->ID;

        // Skip?
        CHECK_CONTINUE(Config.UpdateFrequency < 0.0f && cache.Timestamp > 0.001f);
        CHECK_CONTINUE(InArgs.ContextPtr->Time() - cache.Timestamp < Config.UpdateFrequency)
        Utility::SortedInsert(timeSortedCache, &cache, [&](const LightData* InFirst, const LightData* InSecond)
        {
            return InFirst->Timestamp < InSecond->Timestamp;
        });
    }
    
    CHECK_RETURN(timeSortedCache.empty(), {});

    Array<QuatF, 6> directions = RaylibRenderUtility::GetCubemapRotations();
    Vec2F size = Target.Size().To<float>();
    RenderArgs args = {
        .ScenePtr = InArgs.ScenePtr,
        .ContextPtr = InArgs.ContextPtr,
        .ViewportPtr = &AtlasView,
        .LuminPtr = InArgs.LuminPtr,
        .LightsPtr = this,
        .Perspectives = {}
    };
    
    int count = 0;
    Pipeline::Stats stats;
    for (auto& cache : timeSortedCache) 
    {
        CHECK_CONTINUE(!cache);
        cache->Timestamp = args.ContextPtr->Time();
        cache->Pos = cache->Data.Position;
        
        auto rect = Atlas.GetRect(cache->ID, 0).To<float>();
        cache->Rect = {
            rect.x / size.x,
            rect.y / size.y,
            rect.z / size.x,
            rect.w / size.y
        };
        
        for (int i = 0; i < 6; i++)
        {
            args.Perspectives.push_back({
                .TargetRect = Atlas.GetRect(cache->ID, i),
                .Camera = {
                    .Position = cache->Data.Position,
                    .Rotation = directions[i],
                    .FOV = 90.0f,
                    .Far = cache->Data.Range,
                    .Near = 0.01f
                }
            });
        }
        
        count++;
        if (count >= Config.MaxShadowRenders)
            break;
    }

    AtlasView.BeginFrame();
    stats += Pipeline.RenderShadows(args, Config.CollectShader, Target);

    // TODO: Clear unused lights
    
    return stats;
}

Vector<const LightInstance*> Rendering::Lights::GetLights(const RenderArgs& InArgs)
{
    PROFILE_GL();
    
    CHECK_RETURN(InArgs.Perspectives.empty(), {});
    
    auto& cam = InArgs.Perspectives.at(0).Camera;
    Frustum frustum;
    frustum.ConstructFrustum(cam, AtlasView.GetResolution());
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
    
    for (auto& light : InArgs.Scene->Lights.GetAll())
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

