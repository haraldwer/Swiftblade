#include "Lumin.h"

#include "RayRenderUtility.h"
#include "Scene/Instances/CameraInstance.h"
#include "Scene/Scene.h"
#include "Utility/Collections/SortedInsert.h"
#include "raylib.h"
#include "Scene/Culling/Frustum.h"

void Rendering::Lumin::Init(const ContextConfig& InConfig)
{
    config = InConfig.Lumin;
    viewport.Init(config.Viewport, {});
    context.Init(InConfig);

    int maxProbes = config.MaxLayerCount * config.Layers + config.AtlasPadding;
    atlas.Init(maxProbes, true, viewport.GetSize().x);

    if (target.TryBeginSetup(viewport.GetSize()))
    {
        target.CreateBuffer("TexFrameIrradiance", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1, RL_TEXTURE_FILTER_LINEAR);
        target.CreateBuffer("TexFramePrefilter", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1, RL_TEXTURE_FILTER_LINEAR);
        target.EndSetup();
    }

    for (auto& t : lerpTarget.All())
    {
        if (t.TryBeginSetup(viewport.GetSize()))
        {
            t.CreateBuffer("TexIrradiance", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1, RL_TEXTURE_FILTER_LINEAR);
            t.CreateBuffer("TexPrefilter", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1, RL_TEXTURE_FILTER_LINEAR);
            t.EndSetup();
        }
    }
}

void Rendering::Lumin::Deinit()
{
    probePersistence = {};
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
    
    Pipeline::Stats stats;
    ExpandVolume(InArgs);
    stats += UpdateFallbackProbe(InArgs);
    if (config.Enabled)
    {
        stats += UpdateProbes(InArgs);
        stats += LerpProbes(InArgs);
    }
    return stats;
}

Rendering::LuminRenderData Rendering::Lumin::GetFrameProbes(const RenderArgs &InArgs)
{
    PROFILE_GL();
    
    LuminRenderData data;
    data.fallback = &fallback;

    if (!config.Enabled)
    {
        data.layers.emplace_back();
        return data;    
    }
    
    for (int layerIndex = 0; layerIndex < config.Layers; ++layerIndex)
    {
        int probeStartIndex = static_cast<int>(data.probes.size());
        auto& layer = data.layers.emplace_back();

        // Get all probes in frame
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
            minCoord.pos.x,
            minCoord.pos.y,
            minCoord.pos.z
        };
        layer.size = {
            maxCoord.pos.x - minCoord.pos.x + 1,
            maxCoord.pos.y - minCoord.pos.y + 1,
            maxCoord.pos.z - minCoord.pos.z + 1
        };
        
        int layer_max = config.MaxGridCount / config.Layers;
        int size = Utility::Math::Min(layer.size.x * layer.size.y * layer.size.z, layer_max);
        layer.startIndex = static_cast<int>(data.indices.size());
        layer.endIndex = layer.startIndex + size;
        data.indices.resize(layer.startIndex + size, -1);

        for (int probeIndex = 0; probeIndex < static_cast<int>(frameProbes.size()); ++probeIndex)
        {
            auto& probe = frameProbes[probeIndex];

            // Maybe skip probe based on timestamps?
            CHECK_CONTINUE(probe->atlasTimestamp <= 0); // Not yet on atlas
            CHECK_CONTINUE(probe->renderTimestamp < probe->atlasTimestamp); // Not yet rendered
            
            ProbeCoord coord = probe->coord;
            Vec3I gridPos = {
                coord.pos.x - layer.start.x,
                coord.pos.y - layer.start.y,
                coord.pos.z - layer.start.z,
            };
            int index = gridPos.x + gridPos.y * layer.size.x + gridPos.z * layer.size.x * layer.size.y + layer.startIndex;
            if (index < 0 || index >= layer.endIndex)
                continue;
            data.indices.at(index) = probeIndex + probeStartIndex;
        }
    }

    // Get timestamp for when probe entered view 
    auto& persistence = renderPersistence[InArgs.cullMask];
    persistence.persistence.Begin();
    for (auto& probe : data.probes)
    {
        CHECK_CONTINUE(!probe);
        persistence.persistence.Touch(probe->coord.key);
    }

    for (auto remCoord : persistence.persistence.GetRemoved())
        persistence.times[remCoord] = 0;
    
    data.timestamps.resize(data.probes.size());
    double time = InArgs.contextPtr->Time();
    for (int i = 0; i < static_cast<int>(data.probes.size()); ++i)
    {
        auto coord = data.probes[i]->coord.key;
        auto& timestamp = persistence.times[coord];
        if (persistence.persistence.WasAdded(coord) || timestamp <= 0)
            timestamp = time;
        data.timestamps[i] = timestamp;
    }
    
    // TODO: Keep old probes around until they've faded
    
    return data;
}


Rendering::Pipeline::Stats Rendering::Lumin::UpdateProbes(const RenderArgs& InArgs)
{
    PROFILE_GL();

    Vector<LuminProbe*> frameProbes;
    double time = InArgs.contextPtr->Time();
    for (int layer = config.Layers - 1; layer >= 0; --layer)
    {
        for (auto& probe : GetProbes(InArgs, layer))
        {
            CHECK_CONTINUE(config.UpdateFrequency < 0.0f && probe->renderTimestamp > 0.001f);
            CHECK_CONTINUE(config.Iterations > 0 && probe->iterations >= config.Iterations);
            CHECK_CONTINUE(time - probe->renderTimestamp < config.UpdateFrequency)
            Utility::SortedInsert(frameProbes, probe, [&](const LuminProbe* InFirst, const LuminProbe* InSecond)
            {
                return InFirst->renderTimestamp < InSecond->renderTimestamp;
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
        .particlesPtr = InArgs.particlesPtr,
        .perspectives = {},
        .cullPoints = {},
        .cullMask = static_cast<uint8>(VisibilityMask::LUMIN)
    };

    for (auto probe : frameProbes)
    {
        if (!atlas.Contains(probe->coord.key))
        {
            probe->iterations = 0;
            probe->atlasTimestamp = 0;
            probe->renderTimestamp = 0;
        }
    }
    
    int count = 0;
    for (auto probe : frameProbes)
    {
        if (probe->iterations == 0)
        {
            probe->atlasTimestamp = time; // Just got assigned an atlas
            probe->renderTimestamp = time; // Just rendered for the first time
        }
        
        probe->iterations++;
        probe->rect = atlas.GetRect(probe->coord.key, 0);

        Array<Vec3F, 9> points = GetCullPoints(probe->pos);
        args.cullPoints.insert(args.cullPoints.end(), points.begin(), points.end());
        
        for (int i = 0; i < 6; i++)
        {
            args.perspectives.push_back({
                .referenceRect = probe->rect,
                .targetRect = atlas.GetRect(probe->coord.key, i),
                .camera = {
                    .position = probe->pos,
                    .rotation = directions[i],
                    .fov = 90.0f,
                    .far = config.Far,
                    .near = config.Near
                }
            });
        }
            
        count++;
        if (count >= config.MaxFrameRenders)
            break;
    }
    
    viewport.BeginFrame();
    return LuminPipeline::RenderProbes(args, config.CollectShader, target);
}

Rendering::Pipeline::Stats Rendering::Lumin::UpdateFallbackProbe(const RenderArgs &InArgs)
{
    Vec4F rect = atlas.GetRect(0, 0, true); // Refresh rect
    
    CHECK_RETURN(fallback.iterations > 0, {});
    CHECK_RETURN(InArgs.scenePtr->environments.empty(), {});

    Array<QuatF, 6> directions = RaylibRenderUtility::GetCubemapRotations();
    RenderArgs args = {
        .scenePtr = InArgs.scenePtr,
        .contextPtr = &context,
        .viewportPtr = &viewport,
        .luminPtr = this,
        .lightsPtr = InArgs.lightsPtr,
        .particlesPtr = InArgs.particlesPtr,
        .perspectives = {},
        .cullMask = static_cast<uint8>(VisibilityMask::LUMIN)
    };

    fallback.renderTimestamp = context.Time();
    fallback.iterations++;
    fallback.rect = rect;

    fallback.pos = {};
    if (!probes.empty())
    {
        for (auto& probe : probes)
            fallback.pos += probe.second.pos;
        fallback.pos /= static_cast<float>(probes.size());
    }
    
    for (int i = 0; i < 6; i++)
    {
        args.perspectives.push_back({
            .referenceRect = fallback.rect,
            .targetRect = atlas.GetRect(0, i),
            .camera = {
                .position = fallback.pos,
                .rotation = directions[i],
                .fov = 90.0f,
                .far = config.Far,
                .near = config.Near
            }
        });
    }

    Array<Vec3F, 9> points = GetCullPoints(fallback.pos);
    args.cullPoints.insert(args.cullPoints.end(), points.begin(), points.end());
    
    auto& t = config.Enabled ? target : lerpTarget.Curr();

    viewport.BeginFrame();
    return LuminPipeline::RenderFallbackProbe(args, config.CollectShader, t, config.SceneFallback);
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

    const float halfFar = cam.far / 2.0f; 
    const Vec3F maxDist =  Vec3F::One() / GetDensity(InLayer);
    const float cullDist = Utility::Math::Max(Utility::Math::Max(maxDist.x, maxDist.y), maxDist.z);
    const float cullFar = halfFar - cullDist;
    const auto checkFunc = [&](const ProbeCoord& InCoord)
    {
        const Vec3F pos = FromCoord(InCoord); 
        return frustum.CheckSphere(pos, cullDist) || (pos - cam.position).LengthSqr() < cullFar * cullFar;
    };

    Set<uint64>& layer = layerProbes[InLayer];
    for (auto& id : layer)
    {
        PROFILE_GL_NAMED("Insert layer");
        if (checkFunc(ProbeCoord(id)))
            Utility::SortedInsert(result, &probes.at(id), sortFunc);
    }
    
    const auto count = Utility::Math::Min(static_cast<int>(result.size()), config.MaxLayerCount.Get());
    CHECK_RETURN(count <= 0, {});
    return { result.begin(), result.begin() + count };
}

void Rendering::Lumin::ExpandVolume(const RenderArgs& InArgs)
{
    PROFILE_GL();
    CHECK_RETURN(!config.Enabled);
    CHECK_ASSERT(!InArgs.scenePtr, "Invalid scene");

    bool changed = probes.empty();
    
    struct RangeTrans
    {
        const Utility::SplitContainer<Mat4F>* trans;
        float range = 1.0f;
    };
    
    Vector<RangeTrans> transformCollections;
    for (auto& persistence : InArgs.scenePtr->meshes.GetEntries())
    {
        CHECK_CONTINUE(persistence.first == 0);
        for (const std::pair<const unsigned long, MeshCollection::Entry> &entry: persistence.second)
        {
            CHECK_CONTINUE(!(entry.second.mask & static_cast<uint8>(VisibilityMask::LUMIN)));
            changed |= entry.second.justRebuilt;
            transformCollections.push_back({
                &entry.second.transforms,
                entry.second.model.Get()->GetRange()
            });
        }
    }

    CHECK_RETURN(!changed) // Only recheck if anything was rebuilt
    LOG("Checking lumin volume")
    
    // Find changes
    probePersistence.Begin();
    Vec3F offset = config.Offset;
    
    Vec3F min;
    Vec3F max;
    for (auto& transforms : transformCollections)
    {
        for (const Mat4F& t : transforms.trans->GetAll())
        {
            // TODO: Consider scale
            const Vec3F pos = t.GetPosition();
            min = {
                Utility::Math::Min(min.x, pos.x - transforms.range),
                Utility::Math::Min(min.y, pos.y - transforms.range),
                Utility::Math::Min(min.z, pos.z - transforms.range),
            };
            max = {
                Utility::Math::Max(max.x, pos.x + transforms.range),
                Utility::Math::Max(max.y, pos.y + transforms.range),
                Utility::Math::Max(max.z, pos.z + transforms.range),
            };
        }
    }
    
    for (int layer = 0; layer < config.Layers; layer++)
    {
        const auto density = GetDensity(layer);
        const int16 startx = static_cast<int16>(floorf(min.x * density.x - offset.x));
        const int16 starty = static_cast<int16>(floorf(min.y * density.y - offset.y));
        const int16 startz = static_cast<int16>(floorf(min.z * density.z - offset.z));
        const int16 endx = static_cast<int16>(ceilf(max.x * density.x - offset.x));
        const int16 endy = static_cast<int16>(ceilf(max.y * density.y - offset.y));
        const int16 endz = static_cast<int16>(ceilf(max.z * density.z - offset.z));
        for (int16 x = startx; x <= endx; x++)
            for (int16 y = starty; y <= endy; y++)
                for (int16 z = startz; z <= endz; z++)
                    probePersistence.Touch(ProbeCoord(x, y, z, static_cast<int16>(layer)).key);
    }

    for (auto added : probePersistence.GetAdded())
        CreateProbe({ added });
    for (auto& removed : probePersistence.GetRemoved())
        RemoveProbe({ removed });
    
    // Reset cache, redo every probe
    if (probePersistence.Changed())
    {
        if (config.InvalidateFallback)
            fallback = {};
        if (config.InvalidateCache)
            for (auto& probe : probes)
                probe.second.iterations = 0;
    }
}

void Rendering::Lumin::CreateProbe(const ProbeCoord InCoord)
{
    if (InCoord.key == 0)
        return;
    auto& probe = probes[InCoord.key];
    if (probe.coord.key == 0)
    {
        probe.coord = InCoord;
        probe.pos = FromCoord(InCoord);
        layerProbes[InCoord.pos.w].insert(InCoord.key);
    }
}

void Rendering::Lumin::RemoveProbe(ProbeCoord InCoord)
{
    if (InCoord.key == 0)
        return;
    probes.erase(InCoord.key);
    layerProbes.at(InCoord.pos.w).erase(InCoord.key);
}

Vec3F Rendering::Lumin::GetDensity(const int InLayer) const
{
    return config.BaseDensity.Get() * powf(config.LayerScale.Get(), static_cast<float>(InLayer));
}

Rendering::ProbeCoord Rendering::Lumin::FromPos(const Vec3F& InPos, int InLayer) const
{
    const Vec3F offset = config.Offset;
    const auto density = GetDensity(InLayer);
    return {
        static_cast<int16>(round(InPos.x * density.x - offset.x)),
        static_cast<int16>(round(InPos.y * density.y - offset.y)),
        static_cast<int16>(round(InPos.z * density.z - offset.z)),
        static_cast<int16>(InLayer)
    };
}

Vec3F Rendering::Lumin::FromCoord(const ProbeCoord& InCoord) const
{
    const Vec3F offset = config.Offset;
    const auto density = GetDensity(InCoord.pos.w);
    return {                                
        (static_cast<float>(InCoord.pos.x) + offset.x) / density.x,
        (static_cast<float>(InCoord.pos.y) + offset.y) / density.y,
        (static_cast<float>(InCoord.pos.z) + offset.z) / density.z,
    };
}

Array<Vec3F, 9> Rendering::Lumin::GetCullPoints(const Vec3F &InPos) const
{
    float InFar = config.Far;
    return {
        InPos,
        InPos + Vec3F(InFar, InFar, InFar),
        InPos + Vec3F(InFar, -InFar, InFar),
        InPos + Vec3F(-InFar, -InFar, InFar),
        InPos + Vec3F(-InFar, InFar, InFar),
        InPos + Vec3F(InFar, InFar, -InFar),
        InPos + Vec3F(InFar, -InFar, -InFar),
        InPos + Vec3F(-InFar, -InFar, -InFar),
        InPos + Vec3F(-InFar, InFar, -InFar),
    };
}