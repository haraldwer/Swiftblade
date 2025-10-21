#include "Lights.h"

#include "RayRenderUtility.h"
#include "Context/Context.h"
#include "Pipeline/Renderer.h"
#include "Scene/Scene.h"
#include "Scene/Culling/Frustum.h"
#include "Utility/Collections/SortedInsert.h"

void Rendering::Lights::Init(const LightConfig& InConfig)
{
    config = InConfig;
    atlasView.Init(config.Viewport, {});
    atlas.Init(config.MaxLights, true, atlasView.GetSize().x);
    target.Setup(atlasView.GetSize(), "TexShadow", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
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
        LightData& lightData = cache[light->id];

        // Set data
        lightData.data = light->data;
        lightData.id = light->id;

        // Skip?
        CHECK_CONTINUE(config.UpdateFrequency < 0.0f && lightData.timestamp > 0.001f);
        CHECK_CONTINUE(InArgs.contextPtr->Time() - lightData.timestamp < config.UpdateFrequency)
        Utility::SortedInsert(timeSortedCache, &lightData, [&](const LightData* InFirst, const LightData* InSecond)
        {
            return InFirst->timestamp < InSecond->timestamp;
        });
    }
    
    CHECK_RETURN(timeSortedCache.empty(), {});

    Array<QuatF, 6> directions = RaylibRenderUtility::GetCubemapRotations();
    RenderArgs args = {
        .scenePtr = InArgs.scenePtr,
        .contextPtr = InArgs.contextPtr,
        .viewportPtr = &atlasView,
        .luminPtr = InArgs.luminPtr,
        .lightsPtr = this,
        .perspectives = {},

        // TODO: Cache cull results, or unique culling for each light?  
        .cullPoints = InArgs.cullPoints,
        .cullMask = static_cast<uint8>(VisibilityMask::SHADOWS)
    };
    
    int count = 0;
    Pipeline::Stats stats;
    for (auto& lightData : timeSortedCache) 
    {
        CHECK_CONTINUE(!lightData);
        lightData->timestamp = args.contextPtr->Time();
        lightData->pos = lightData->data.position;
        lightData->rect = atlas.GetRect(lightData->id, 0).To<float>();
        
        for (int i = 0; i < 6; i++)
        {
            args.perspectives.push_back({
                .targetRect = atlas.GetRect(lightData->id, i),
                .camera = {
                    .position = lightData->data.position,
                    .rotation = directions[i],
                    .fov = 90.0f,
                    .far = lightData->data.range,
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
    return stats;
}

Vector<const Rendering::LightInstance*> Rendering::Lights::GetLights(const RenderArgs& InArgs)
{
    PROFILE_GL();
    
    CHECK_RETURN(InArgs.perspectives.empty(), {});
    
    auto& cam = InArgs.perspectives.at(0).camera;
    auto res = atlasView.GetResolution();
    auto points = cam.GetFrustumCorners(res.To<float>());
    
    // Query spatialContainer
    auto culled = InArgs.scenePtr->lights.GetIndices(points);
    auto sortFunc = [&](const LightInstance* InFirst, const LightInstance* InSecond)
    {
        return (InFirst->data.position - cam.position).LengthSqr() < (InSecond->data.position - cam.position).LengthSqr();
    };
    
    Frustum frustum;
    frustum.ConstructFrustum(cam, res);
    Vector<const LightInstance*> result;

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

    auto& data = InArgs.scenePtr->lights.GetAll();
    for (auto& index : culled)
        if (checkFunc(data.at(index)))
            Utility::SortedInsert(result, &data.at(index), sortFunc);

    const auto count = Utility::Math::Min(static_cast<int>(result.size()), config.MaxLights.Get());
    CHECK_RETURN(count <= 0, {});
    return { result.begin(), result.begin() + count }; 
}

const Rendering::LightData& Rendering::Lights::GetData(const uint32 InHash)
{
    return cache.at(InHash);
}

Vec2F Rendering::Lights::GetFaceTexel() const
{
    return Vec2F(1.0f) / static_cast<float>(atlas.GetSlotSize());
}

