#include "Lumin.h"

#include "RayRenderUtility.h"
#include "Scene/Instances/CameraInstance.h"
#include "Scene/Scene.h"
#include "Utility/Collections/SortedInsert.h"
#include "raylib.h"
#include "Scene/Culling/Frustum.h"

void Rendering::Lumin::Init(const LuminConfig& InConfig)
{
    config = InConfig;
    viewport.Init(config.Viewport);
    context.Init(config.Context, InConfig);

    int maxProbes = config.MaxLayerCount * config.Layers + 1;
    atlas.Init(maxProbes + config.AtlasPadding, true);

    if (target.TryBeginSetup(viewport.GetVirtualTarget()))
    {
        target.CreateBuffer("TexFrameIrradiance", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
        target.CreateBuffer("TexFramePrefilter", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
        target.EndSetup(viewport.GetVirtualTarget());
    }

    if (lerpTarget.TryBeginSetup(viewport.GetVirtualTarget()))
    {
        lerpTarget.CreateBuffer("TexIrradiance", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
        lerpTarget.CreateBuffer("TexPrefilter", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
        lerpTarget.EndSetup(viewport.GetVirtualTarget());
    }
}

void Rendering::Lumin::Deinit()
{
    fallback = {};
    probes.clear();
    layerProbes.clear();
    viewport.Deinit();
    context.Deinit();
    atlas.Deinit();
    target.Unload();
    lerpTarget.Unload();
}

Rendering::Pipeline::Stats Rendering::Lumin::Update(const RenderArgs& InArgs)
{
    PROFILE_GL();
    
    CHECK_ASSERT(!InArgs.scenePtr, "Invalid scene");
    CHECK_ASSERT(!InArgs.viewportPtr, "Invalid viewport");

    if (auto tex = config.TexBRDF.Get().Get())
        if (!tex->IsBaked())
            tex->Bake();
    
    ExpandVolume(*InArgs.scenePtr);
    Pipeline::Stats stats;
    stats += UpdateProbes(InArgs);
    stats += UpdateFallbackProbe(InArgs);
    stats += LerpProbes(InArgs);
    return stats;
}

Rendering::LuminRenderData Rendering::Lumin::GetFrameProbes(const RenderArgs &InArgs)
{
    LuminRenderData data;
    for (int layerIndex = 0 ; layerIndex < config.Layers; ++layerIndex)
    {
        int startIndex = data.probes.size();
        auto& layer = data.layers.emplace_back();
        Vector<LuminProbe*> frameProbes = GetProbes(InArgs, layerIndex);
        data.probes.insert(data.probes.end(), frameProbes.begin(), frameProbes.end());
        
        Vec3F min = Vec3F::Zero();
        Vec3F max = Vec3F::Zero();
        for (auto& probe : frameProbes)
        {
            if (min == Vec3F::Zero() && max == Vec3F::Zero())
            {
                min = probe->pos;
                max = probe->pos;
                continue;
            }

            min.x = Utility::Math::Min(min.x, probe->pos.x);
            min.y = Utility::Math::Min(min.y, probe->pos.y);
            min.z = Utility::Math::Min(min.z, probe->pos.z);
            max.x = Utility::Math::Max(max.x, probe->pos.x);
            max.y = Utility::Math::Max(max.y, probe->pos.y);
            max.z = Utility::Math::Max(max.z, probe->pos.z);
        }

        layer.density = GetDensity(layerIndex);
        
        Vec3F minGrid = (min * layer.density); 
        Vec3F sizeGrid = (max - min) * layer.density;
        
        layer.start = Vec3F({
            floor(minGrid.x),
            floor(minGrid.y),
            floor(minGrid.z),
        }).To<int>();
        layer.size = Vec3F({
            ceil(sizeGrid.x),
            ceil(sizeGrid.y),
            ceil(sizeGrid.z),
        }).To<int>() + Vec3I::One();
           
        int layer_max = config.MaxGridCount / config.Layers;
        int size = Utility::Math::Min(layer.size.x * layer.size.y * layer.size.z, layer_max);
        layer.indices.resize(size, -1);

        for (int probeIndex = 0; probeIndex < static_cast<int>(frameProbes.size()); ++probeIndex)
        {
            auto p = frameProbes[probeIndex]->coord;
            Vec3I c = {
                p.x - layer.start.x,
                p.y - layer.start.y,
                p.z - layer.start.z,
            };
            int index = c.x + c.y * layer.size.x + c.z * layer.size.x * layer.size.y;
            layer.indices.at(index) = probeIndex + startIndex;
        }
    }
    data.fallback = &fallback;
    return data;
}

Rendering::Pipeline::Stats Rendering::Lumin::UpdateProbes(const RenderArgs& InArgs)
{
    PROFILE_GL();

    Vector<LuminProbe*> frameProbes;
    for (int layer = 0; layer < config.Layers; ++layer)
    {
        for (auto& probe : GetProbes(InArgs, layer))
        {
            CHECK_CONTINUE(config.UpdateFrequency < 0.0f && probe->timestamp > 0.001f);
            CHECK_CONTINUE(config.Iterations > 0 && probe->iterations >= config.Iterations);
            CHECK_CONTINUE(InArgs.contextPtr->Time() - probe->timestamp < config.UpdateFrequency)
            Utility::SortedInsert(frameProbes, probe, [&](const LuminProbe* InFirst, const LuminProbe* InSecond)
            {
                return InFirst->timestamp < InSecond->timestamp;
            });
        }
    }
    
    CHECK_RETURN(frameProbes.empty(), {});

    Array<QuatF, 6> directions = RaylibRenderUtility::GetCubemapRotations();
    RenderArgs args = {
        .scenePtr = InArgs.scenePtr,
        .contextPtr = &context,
        .viewportPtr = &viewport,
        .luminPtr = this,
        .lightsPtr = InArgs.lightsPtr,
        .perspectives = {},
        .cullPoints = InArgs.cullPoints,
        .cullMask = static_cast<uint8>(MeshMask::LUMIN)
    };

    int count = 0;
    for (auto probe : frameProbes)
    {
        CHECK_CONTINUE(!probe);
        probe->timestamp = context.Time();
        probe->iterations++;
        probe->rect = atlas.GetRect(probe->coord.id, 0);
        args.cullPoints.push_back(probe->pos);
        
        for (int i = 0; i < 6; i++)
        {
            args.perspectives.push_back({
                .referenceRect = probe->rect,
                .targetRect = atlas.GetRect(probe->coord.id, i),
                .camera = {
                    .position = probe->pos,
                    .rotation = directions[i],
                    .fov = 90.0f,
                    .far = 20.0f,
                    .near = 0.01f
                }
            });
        }
            
        count++;
        if (count >= config.MaxFrameRenders)
            break;
    }
    
    viewport.BeginFrame();
    return pipeline.RenderProbes(args, config.CollectShader, target);
}

Rendering::Pipeline::Stats Rendering::Lumin::UpdateFallbackProbe(const RenderArgs &InArgs)
{
    Vec4F rect = atlas.GetRect(0, 0); // Refresh rect
    if (fallback.iterations > 0)
        return {};

    Array<QuatF, 6> directions = RaylibRenderUtility::GetCubemapRotations();
    RenderArgs args = {
        .scenePtr = InArgs.scenePtr,
        .contextPtr = &context,
        .viewportPtr = &viewport,
        .luminPtr = this,
        .lightsPtr = InArgs.lightsPtr,
        .perspectives = {},
        .cullMask = static_cast<uint8>(MeshMask::NONE)
    };

    fallback.timestamp = context.Time();
    fallback.iterations++;
    fallback.rect = rect;
    
    for (int i = 0; i < 6; i++)
    {
        args.perspectives.push_back({
            .referenceRect = fallback.rect,
            .targetRect = atlas.GetRect(0, i),
            .camera = {
                .position = Vec3F::Zero(),
                .rotation = directions[i],
                .fov = 90.0f,
                .far = InArgs.scenePtr->GetCamera().far,
                .near = 0.1f
            }
        });
    }
    
    viewport.BeginFrame();
    return pipeline.RenderFallbackProbe(args, config.CollectShader, target);
}

Rendering::Pipeline::Stats Rendering::Lumin::LerpProbes(const RenderArgs& InArgs)
{
    const RenderArgs lerpArgs = {
        .scenePtr = InArgs.scenePtr,
        .contextPtr = &context,
        .viewportPtr = &viewport,
        .luminPtr = this,
        .lightsPtr = InArgs.lightsPtr,
        .perspectives = {{
            .referenceRect= Vec4F(),
            .targetRect= Vec4F(),
            .camera= InArgs.scenePtr->GetCamera()
        }}
    };
    return LuminPipeline::LerpProbes(lerpArgs, config.LerpShader, target, lerpTarget);
}

Vector<Rendering::LuminProbe*> Rendering::Lumin::GetProbes(const RenderArgs& InArgs, int InLayer)
{
    PROFILE_GL();
    
    CHECK_RETURN(InArgs.perspectives.empty(), {})

    const CameraInstance& cam = InArgs.perspectives.at(0).camera;
    Frustum frustum;
    frustum.ConstructFrustum(cam, viewport.GetResolution());
    Vector<LuminProbe*> result;

    const auto sortFunc = [&](const LuminProbe* InFirst, const LuminProbe* InSecond)
    {
        return (InFirst->pos - cam.position).LengthSqr() < (InSecond->pos - cam.position).LengthSqr();
    };

    const auto checkFunc = [&](const ProbeCoord& InCoord)
    {
        const Vec3F pos = FromCoord(InCoord);
        const Vec3F maxDist = Vec3F::One() / GetDensity(InLayer);
        const float cullDist = Utility::Math::Max(Utility::Math::Max(maxDist.x, maxDist.y), maxDist.z);
        if (frustum.CheckSphere(pos, cullDist))
            return true;
        return false;
    };

    Vector<ProbeCoord>& layer = layerProbes[InLayer];
    for (auto& probeCoord : layer)
        if (checkFunc(probeCoord))
            Utility::SortedInsert(result, &probes.at(probeCoord.id), sortFunc);

    const auto count = Utility::Math::Min(static_cast<int>(result.size()), config.MaxLayerCount.Get());
    CHECK_RETURN(count <= 0, {});
    return { result.begin(), result.begin() + count };
}

void Rendering::Lumin::ExpandVolume(const Scene& InScene)
{
    PROFILE_GL();

    bool changed = false;
    for (auto& persistence : InScene.meshes.GetEntries())
    {
        CHECK_CONTINUE(persistence.first == 0);
        for (auto& entry : persistence.second)
        {
            CHECK_CONTINUE(!entry.second.justRebuilt);
            for (auto& trans : entry.second.transforms.GetAll())
            {
                for (int i = 0; i < config.Layers; i++)
                {
                    const ProbeCoord coord = FromPos(trans.GetPosition(), i);
                    TryCreateProbe(coord);
                    changed = true;
                }
            }
        }
    }

    // Reset cache, redo every probe
    if (changed && config.ResetCache)
    {
        fallback = {};
        for (auto& probe : probes)
            probe.second.iterations = 0;
    }
}

void Rendering::Lumin::TryCreateProbe(const ProbeCoord InCoord)
{
    if (InCoord.id == 0)
        return;
    auto& probe = probes[InCoord.id];
    if (probe.coord.id == 0)
    {
        probe.coord = InCoord;
        probe.pos = FromCoord(InCoord);
        layerProbes[InCoord.layer].push_back(InCoord);
    }
}

Vec3F Rendering::Lumin::GetDensity(const int InLayer) const
{
    return config.BaseDensity.Get() * powf(config.LayerScale.Get(), static_cast<float>(InLayer));
}

Rendering::ProbeCoord Rendering::Lumin::FromPos(const Vec3F& InPos, int InLayer) const
{
    const auto density = GetDensity(InLayer);
    return {
        .x = static_cast<int16>(InPos.x * density.x - 0.5f),
        .y = static_cast<int16>(InPos.y * density.y - 0.5f),
        .z = static_cast<int16>(InPos.z * density.z - 0.5f),
        .layer = static_cast<int16>(InLayer)
    };
}

Vec3F Rendering::Lumin::FromCoord(const ProbeCoord& InCoord) const
{
    const auto density = GetDensity(InCoord.layer);
    return {                                
        (static_cast<float>(InCoord.x) + 0.5f) / density.x,
        (static_cast<float>(InCoord.y) + 0.5f) / density.y,
        (static_cast<float>(InCoord.z) + 0.5f) / density.z,
    };
}
