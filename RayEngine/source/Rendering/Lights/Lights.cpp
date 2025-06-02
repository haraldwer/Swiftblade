#include "Lights.h"

#include "Context/Context.h"
#include "Pipeline/Renderer.h"
#include "RayRenderUtility.h"
#include "Scene/Scene.h"
#include "Utility/Collections/SortedInsert.h"

void Rendering::Lights::Init(const LightConfig& InConfig)
{
    config = InConfig;
    atlasView.Init(config.Viewport);
    atlas.Init(atlasView.GetResolution(), config.MaxLights, true);
    target.Setup(atlasView.GetVirtualTarget(), "TexShadow", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
}

void Rendering::Lights::Deinit()
{
    atlasView.Deinit();
    atlas.Deinit();
    cache.clear();
    target.Unload();
}

Rendering::Pipeline::Stats Rendering::Lights::Update(const RenderArgs& InArgs)
{
    PROFILE_GL();
    
    CHECK_ASSERT(!InArgs.scenePtr, "Invalid scene");
    CHECK_ASSERT(!InArgs.viewportPtr, "Invalid viewport");
    
    // Only consider lights in view, ordered by distance
    Vector<const LightInstance*> frameLights = GetLights(InArgs);

    // Filter and sort by timestamp
    Vector<LightData*> timeSortedCache;
    for (auto& light : frameLights)
    {
        CHECK_ASSERT(light->id == 0, "Invalid hash");
        LightData& cache = cache[light->id];

        // Set data
        cache.data = light->data;
        cache.id = light->id;

        // Skip?
        CHECK_CONTINUE(config.UpdateFrequency < 0.0f && cache.timestamp > 0.001f);
        CHECK_CONTINUE(InArgs.contextPtr->Time() - cache.timestamp < config.UpdateFrequency)
        Utility::SortedInsert(timeSortedCache, &cache, [&](const LightData* InFirst, const LightData* InSecond)
        {
            return InFirst->timestamp < InSecond->timestamp;
        });
    }
    
    CHECK_RETURN(timeSortedCache.empty(), {});

    Array<QuatF, 6> directions = RaylibRenderUtility::GetCubemapRotations();
    Vec2F size = target.Size().To<float>();
    RenderArgs args = {
        .scenePtr = InArgs.scenePtr,
        .contextPtr = InArgs.contextPtr,
        .viewportPtr = &atlasView,
        .luminPtr = InArgs.luminPtr,
        .lightsPtr = this,
        .perspectives = {}
    };
    
    int count = 0;
    Pipeline::Stats stats;
    for (auto& cache : timeSortedCache) 
    {
        CHECK_CONTINUE(!cache);
        cache->timestamp = args.contextPtr->Time();
        cache->pos = cache->data.position;
        
        auto rect = atlas.GetRect(cache->id, 0).To<float>();
        cache->rect = {
            rect.x / size.x,
            rect.y / size.y,
            rect.z / size.x,
            rect.w / size.y
        };
        
        for (int i = 0; i < 6; i++)
        {
            args.perspectives.push_back({
                .targetRect = atlas.GetRect(cache->id, i),
                .camera = {
                    .position = cache->data.position,
                    .rotation = directions[i],
                    .fov = 90.0f,
                    .far = cache->data.range,
                    .near = 0.01f
                }
            });
        }
        
        count++;
        if (count >= config.MaxShadowRenders)
            break;
    }

    atlasView.BeginFrame();
    stats += pipeline.RenderShadows(args, config.CollectShader, target);

    // TODO: Clear unused lights
    
    return stats;
}

Vector<const LightInstance*> Rendering::Lights::GetLights(const RenderArgs& InArgs)
{
    PROFILE_GL();
    
    CHECK_RETURN(InArgs.perspectives.empty(), {});
    
    auto& cam = InArgs.perspectives.at(0).camera;
    Frustum frustum;
    frustum.ConstructFrustum(cam, atlasView.GetResolution());
    Vector<const LightInstance*> result;

    auto sortFunc = [&](const LightInstance* InFirst, const LightInstance* InSecond)
    {
        return (InFirst->data.position - cam.position).LengthSqr() < (InSecond->data.position - cam.position).LengthSqr();
    };

    auto checkFunc = [&](const LightInstance& InLight)
    {
        const float camDist = (InLight.data.position - cam.position).LengthSqr();
        if (camDist < InLight.data.range * InLight.data.range)
            return true;
        const float cullDist = InLight.data.range;
        if (frustum.CheckSphere(InLight.data.position, cullDist))
            return true;
        return false;
    };
    
    for (auto& light : InArgs.scenePtr->lights.GetAll())
        if (checkFunc(light))
            Utility::SortedInsert(result, &light, sortFunc);

    const auto count = Utility::Math::Min(static_cast<int>(result.size()), config.MaxLights.Get());
    CHECK_RETURN(count <= 0, {});
    return { result.begin(), result.begin() + count }; 
}

const Rendering::LightData& Rendering::Lights::GetData(const uint32 InHash)
{
    return cache.at(InHash);
}

