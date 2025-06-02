#include "Lumin.h"

#include "RayRenderUtility.h"
#include "Scene/Instances/CameraInstance.h"
#include "Scene/Scene.h"
#include "Utility/Collections/SortedInsert.h"
#include "raylib.h"

void Rendering::Lumin::Init(const LuminConfig& InConfig)
{
    config = InConfig;
    viewport.Init(config.Viewport);
    context.Init(config.Context, InConfig);
    atlasMap.Init(viewport.GetResolution(), config.MaxProbes + config.AtlasPadding, true);
    target.Setup(viewport.GetVirtualTarget(), "TexLuminCollect", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    for (auto& t : lerpTarget.All())
        t.Setup(viewport.GetVirtualTarget(), "TexLumin", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
}

void Rendering::Lumin::Deinit()
{
    probes.clear();
    viewport.Deinit();
    context.Deinit();
    atlasMap.Deinit();
    target.Unload();
    for (auto& t : lerpTarget.All())
        t.Unload();
}

Rendering::Pipeline::Stats Rendering::Lumin::Update(const RenderArgs& InArgs)
{
    PROFILE_GL();
    
    CHECK_ASSERT(!InArgs.scenePtr, "Invalid scene");
    CHECK_ASSERT(!InArgs.viewportPtr, "Invalid viewport");

    ExpandVolume(*InArgs.scenePtr);
    Pipeline::Stats stats;
    stats += UpdateProbes(InArgs);
    stats += LerpProbes(InArgs);
    return stats;
}

float Rendering::Lumin::GetRange() const
{
    const Vec3F range = Vec3F::One() * config.RangeMultiplier.Get() / config.Density.Get();
    const float maxRange = Utility::Math::Max(Utility::Math::Max(range.x, range.y), range.z);
    return maxRange;
}

Rendering::Pipeline::Stats Rendering::Lumin::UpdateProbes(const RenderArgs& InArgs)
{
    PROFILE_GL();
    
    Vector<LuminProbe*> frameProbes = GetProbes(InArgs);
    Vector<LuminProbe*> timeSorted;
    for (auto& probe : frameProbes)
    {
        CHECK_CONTINUE(config.UpdateFrequency < 0.0f && probe->timestamp > 0.001f);
        CHECK_CONTINUE(config.Iterations > 0 && probe->iterations >= config.Iterations);
        CHECK_CONTINUE(InArgs.contextPtr->Time() - probe->timestamp < config.UpdateFrequency)
        Utility::SortedInsert(timeSorted, probe, [&](const LuminProbe* InFirst, const LuminProbe* InSecond)
        {
            // TODO: Also consider distance
            return InFirst->timestamp < InSecond->timestamp;
        });
    }

    CHECK_RETURN(timeSorted.empty(), {});

    Array<QuatF, 6> directions = RaylibRenderUtility::GetCubemapRotations();
    Vec2F size = target.Size().To<float>();
    RenderArgs args = {
        .scenePtr = InArgs.scenePtr,
        .contextPtr = &context,
        .viewportPtr = &viewport,
        .luminPtr = this,
        .lightsPtr = InArgs.lightsPtr,
        .perspectives = {}
    };
    float range = GetRange();

    int count = 0;
    for (auto probe : timeSorted)
    {
        CHECK_CONTINUE(!probe);
        probe->timestamp = context.Time();
        probe->iterations++;

        auto rect = atlasMap.GetRect(probe->coord.id, 0).To<float>();
        probe->rect = {
            rect.x / size.x,
            rect.y / size.y,
            rect.z / size.x,
            rect.w / size.y
        };
        
        for (int i = 0; i < 6; i++)
        {
            args.perspectives.push_back({
                .referenceRect = rect,
                .targetRect = atlasMap.GetRect(probe->coord.id, i),
                .camera = {
                    .position = probe->pos,
                    .rotation = directions[i],
                    .fov = 90.0f,
                    .far = range * 2,
                    .near = 0.1f
                }
            });
        }
        
        count++;
        if (count >= config.MaxProbeRenders)
            break;
    }

    viewport.BeginFrame();
    return pipeline.RenderProbes(args, config.CollectShader, target);
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
            .targetRect= Vec4I(),
            .camera= InArgs.scenePtr->GetCamera()
        }}
    };
    return pipeline.LerpProbes(lerpArgs, config.LerpShader, target, lerpTarget);
}

Vector<Rendering::LuminProbe*> Rendering::Lumin::GetProbes(const RenderArgs& InArgs)
{
    PROFILE_GL();
    
    CHECK_RETURN(InArgs.perspectives.empty(), {})
    
    auto& cam = InArgs.perspectives.at(0).camera;
    Frustum frustum;
    frustum.ConstructFrustum(cam, viewport.GetResolution());
    Vector<LuminProbe*> result;

    auto sortFunc = [&](const LuminProbe* InFirst, const LuminProbe* InSecond)
    {
        return (InFirst->pos - cam.position).LengthSqr() < (InSecond->pos - cam.position).LengthSqr();
    };

    auto checkFunc = [&](const LuminProbe& InProbe)
    {
        const Vec3F maxDist = (Vec3F(1.0f) / config.Density.Get()) * config.CullMultiplier.Get();
        const float cullDist = Utility::Math::Max(Utility::Math::Max(maxDist.x, maxDist.y), maxDist.z);
        if (frustum.CheckSphere(InProbe.pos, cullDist))
            return true;
        return false;
    };
    
    for (auto& probe : probes)
        if (checkFunc(probe.second))
            Utility::SortedInsert(result, &probe.second, sortFunc);

    const auto count = Utility::Math::Min(static_cast<int>(result.size()), config.MaxProbes.Get());
    CHECK_RETURN(count <= 0, {});
    return { result.begin(), result.begin() + count };
}

void Rendering::Lumin::ExpandVolume(const Scene& InScene)
{
    PROFILE_GL();
    
    for (auto& entry : InScene.meshes.entries)
    {
        for (auto& trans : entry.second.transforms.GetAll())
        {
            // Get extent, maybe add coord?
            const ProbeCoord coord = FromPos(trans.GetPosition());
            TryCreateProbe(coord);
        }
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
    }
}

Rendering::ProbeCoord Rendering::Lumin::FromPos(const Vec3F& InPos)
{
    const auto density = config.Density.Get();
    return {
        .x = static_cast<int16>(InPos.x * density.x - 0.5f),
        .y = static_cast<int16>(InPos.y * density.y - 0.5f),
        .z = static_cast<int16>(InPos.z * density.z - 0.5f)
    };
}

Vec3F Rendering::Lumin::FromCoord(const ProbeCoord& InCoord)
{
    const auto density = config.Density.Get();
    return {                                
        (static_cast<float>(InCoord.x) + 0.5f) / density.x,
        (static_cast<float>(InCoord.y) + 0.5f) / density.y,
        (static_cast<float>(InCoord.z) + 0.5f) / density.z,
    };
}
