#include "Lumin.h"

#include "RayRenderUtility.h"
#include "Scene/Instances/CameraInstance.h"
#include "Scene/Scene.h"
#include "Utility/Collections/SortedInsert.h"
#include "raylib.h"
#include "Scene/Culling/Frustum.h"
#include "State/State.h"

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
        target.CreateBuffer("TexFramePrefilter", PIXELFORMAT_UNCOMPRESSED_R8G8B8);
        target.EndSetup(viewport.GetVirtualTarget());
    }

    for (auto& t : lerpTarget.All())
    {
        if (t.TryBeginSetup(viewport.GetVirtualTarget()))
        {
            t.CreateBuffer("TexIrradiance", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
            t.CreateBuffer("TexPrefilter", PIXELFORMAT_UNCOMPRESSED_R8G8B8);
            t.EndSetup(viewport.GetVirtualTarget());
        }
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
    for (auto& t : lerpTarget.All())
        t.Unload();
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

    rlState::current.Reset();
    
    return stats;
}

Rendering::LuminRenderData Rendering::Lumin::GetFrameProbes(const RenderArgs &InArgs)
{
    LuminRenderData data;
    for (int layerIndex = 0 ; layerIndex < config.Layers; ++layerIndex)
    {
        int probeStartIndex = static_cast<int>(data.probes.size());
        auto& layer = data.layers.emplace_back();
        Vector<LuminProbe*> frameProbes = GetProbes(InArgs, layerIndex);
        data.probes.insert(data.probes.end(), frameProbes.begin(), frameProbes.end());
        
        Vec3F min = Vec3F::Zero();
        Vec3F max = Vec3F::Zero();
        for (LuminProbe* probe: frameProbes)
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

        ProbeCoord minCoord = FromPos(min, layerIndex);
        ProbeCoord maxCoord = FromPos(max, layerIndex);
        
        layer.density = GetDensity(layerIndex);
        layer.start = {
            minCoord.x,
            minCoord.y,
            minCoord.z
        };
        layer.size = {
            maxCoord.x - minCoord.x + 1,
            maxCoord.y - minCoord.y + 1,
            maxCoord.z - minCoord.z + 1
        };
        
        int layer_max = config.MaxGridCount / config.Layers;
        int size = Utility::Math::Min(layer.size.x * layer.size.y * layer.size.z, layer_max);
        layer.startIndex = static_cast<int>(data.indices.size());
        layer.endIndex = layer.startIndex + size;
        data.indices.resize(layer.startIndex + size, -1);

        for (int probeIndex = 0; probeIndex < static_cast<int>(frameProbes.size()); ++probeIndex)
        {
            ProbeCoord coord = frameProbes[probeIndex]->coord;
            Vec3I gridPos = {
                coord.x - layer.start.x,
                coord.y - layer.start.y,
                coord.z - layer.start.z,
            };
            int index = gridPos.x + gridPos.y * layer.size.x + gridPos.z * layer.size.x * layer.size.y;

            // Now try to calculate the index like in the shader
            //auto coord = FromPos(frameProbes[probeIndex]->pos, layerIndex);
            //Vec3I realCoord = {
            //    coord.x - layer.start.x,
            //    coord.y - layer.start.y,
            //    coord.z - layer.start.z,
            //};
            //int realIndex = realCoord.x + realCoord.y * layer.size.x + realCoord.z * layer.size.x * layer.size.y;
            //LOG("I: " + Utility::ToStr(index - realIndex))
            
            index += layer.startIndex;
            if (index < 0 || index >= static_cast<int>(data.indices.size()))
                continue;
            data.indices.at(index) = probeIndex + probeStartIndex;
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
        .cullPoints = {},
        .cullMask = static_cast<uint8>(MeshMask::LUMIN)
    };

    int count = 0;
    for (auto probe : frameProbes)
    {
        CHECK_CONTINUE(!probe);
        probe->timestamp = context.Time();
        probe->iterations++;
        probe->rect = atlas.GetRect(probe->coord.id, 0);

        float far = InArgs.scenePtr->GetCamera().far;
        Array<Vec3F, 9> points = {
            probe->pos,
            probe->pos + Vec3F(far, far, far),
            probe->pos + Vec3F(far, -far, far),
            probe->pos + Vec3F(-far, -far, far),
            probe->pos + Vec3F(-far, far, far),
            probe->pos + Vec3F(far, far, -far),
            probe->pos + Vec3F(far, -far, -far),
            probe->pos + Vec3F(-far, -far, -far),
            probe->pos + Vec3F(-far, far, -far),
        };
        args.cullPoints.insert(args.cullPoints.end(), points.begin(), points.end());
        
        for (int i = 0; i < 6; i++)
        {
            args.perspectives.push_back({
                .referenceRect = probe->rect,
                .targetRect = atlas.GetRect(probe->coord.id, i),
                .camera = {
                    .position = probe->pos,
                    .rotation = directions[i],
                    .fov = 90.0f,
                    .far = far,
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
    
    CHECK_RETURN(fallback.iterations > 0, {});
    CHECK_RETURN(InArgs.scenePtr->environments.empty(), {});

    Array<QuatF, 6> directions = RaylibRenderUtility::GetCubemapRotations();
    RenderArgs args = {
        .scenePtr = InArgs.scenePtr,
        .contextPtr = &context,
        .viewportPtr = &viewport,
        .luminPtr = this,
        .lightsPtr = InArgs.lightsPtr,
        .perspectives = {},
        .cullMask = static_cast<uint8>(MeshMask::LUMIN)
    };

    fallback.timestamp = context.Time();
    fallback.iterations++;
    fallback.rect = rect;
    
    for (auto& probe : probes)
        fallback.pos += probe.second.pos;
    fallback.pos /= static_cast<float>(probes.size());
    
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

    float far = InArgs.scenePtr->GetCamera().far;
    Array<Vec3F, 9> points = {
        fallback.pos,
        fallback.pos + Vec3F(far, far, far),
        fallback.pos + Vec3F(far, -far, far),
        fallback.pos + Vec3F(-far, -far, far),
        fallback.pos + Vec3F(-far, far, far),
        fallback.pos + Vec3F(far, far, -far),
        fallback.pos + Vec3F(far, -far, -far),
        fallback.pos + Vec3F(-far, -far, -far),
        fallback.pos + Vec3F(-far, far, -far),
    };
    args.cullPoints.insert(args.cullPoints.end(), points.begin(), points.end());
    
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
        return frustum.CheckSphere(pos, cullDist);
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
            CHECK_CONTINUE(!(entry.second.mask & static_cast<uint8>(MeshMask::LUMIN)));
            for (int i = 0; i < config.Layers; i++)
            {
                const auto density = GetDensity(i);
                for (auto& trans : entry.second.transforms.GetAll())
                {
                    Vec3F pos = trans.GetPosition();
                    ProbeCoord ceil {
                        .x = static_cast<int16>(ceilf(pos.x * density.x)),
                        .y = static_cast<int16>(ceilf(pos.y * density.y)),
                        .z = static_cast<int16>(ceilf(pos.z * density.z)),
                        .layer = static_cast<int16>(i)
                    };
                    ProbeCoord floor {
                        .x = static_cast<int16>(floorf(pos.x * density.x)),
                        .y = static_cast<int16>(floorf(pos.y * density.y)),
                        .z = static_cast<int16>(floorf(pos.z * density.z)),
                        .layer = static_cast<int16>(i)
                    };
                    ProbeCoord coords[8] =
                    {
                        { ceil.x, ceil.y, ceil.z, ceil.layer },
                        { floor.x, ceil.y, ceil.z, ceil.layer },
                        { ceil.x, floor.y, ceil.z, ceil.layer },
                        { floor.x, floor.y, ceil.z, ceil.layer },
                        { ceil.x, ceil.y, floor.z, ceil.layer },
                        { floor.x, ceil.y, floor.z, ceil.layer },
                        { ceil.x, floor.y, floor.z, ceil.layer },
                        { floor.x, floor.y, floor.z, ceil.layer },
                    };

                    for (auto& coord : coords)
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
        .x = static_cast<int16>(round(InPos.x * density.x)),
        .y = static_cast<int16>(round(InPos.y * density.y)),
        .z = static_cast<int16>(round(InPos.z * density.z)),
        .layer = static_cast<int16>(InLayer)
    };
}

Vec3F Rendering::Lumin::FromCoord(const ProbeCoord& InCoord) const
{
    const auto density = GetDensity(InCoord.layer);
    return {                                
        static_cast<float>(InCoord.x) / density.x,
        static_cast<float>(InCoord.y) / density.y,
        static_cast<float>(InCoord.z) / density.z,
    };
}
