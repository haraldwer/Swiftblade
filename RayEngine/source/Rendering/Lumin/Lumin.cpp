#include "Lumin.h"

#include "RayRenderUtility.h"
#include "Scene/Instances/CameraInstance.h"
#include "Scene/Scene.h"
#include "Utility/Collections/SortedInsert.h"
#include "raylib.h"

void Rendering::Lumin::Init(const LuminConfig& InConfig)
{
    Config = InConfig;
    Viewport.Init(Config.Viewport);
    Context.Init(Config.Context, InConfig);
    AtlasMap.Init(Viewport.GetResolution(), Config.MaxProbes + Config.AtlasPadding, true);
    Target.Setup(Viewport.GetVirtualTarget(), "TexLuminCollect", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    for (auto& t : LerpTarget.All())
        t.Setup(Viewport.GetVirtualTarget(), "TexLumin", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
}

void Rendering::Lumin::Deinit()
{
    Probes.clear();
    Viewport.Deinit();
    Context.Deinit();
    AtlasMap.Deinit();
    Target.Unload();
    for (auto& t : LerpTarget.All())
        t.Unload();
}

Rendering::Pipeline::Stats Rendering::Lumin::Update(const RenderArgs& InArgs)
{
    PROFILE_GL();
    
    CHECK_ASSERT(!InArgs.ScenePtr, "Invalid scene");
    CHECK_ASSERT(!InArgs.ViewportPtr, "Invalid viewport");

    ExpandVolume(*InArgs.ScenePtr);
    Pipeline::Stats stats;
    stats += UpdateProbes(InArgs);
    stats += LerpProbes(InArgs);
    return stats;
}

float Rendering::Lumin::GetRange() const
{
    Vec3F range = Vec3F::One() * Config.RangeMultiplier.Get() / Config.Density.Get();
    float maxRange = Utility::Math::Max(Utility::Math::Max(range.x, range.y), range.z);
    return maxRange;
}

Rendering::Pipeline::Stats Rendering::Lumin::UpdateProbes(const RenderArgs& InArgs)
{
    PROFILE_GL();
    
    Vector<LuminProbe*> frameProbes = GetProbes(InArgs);
    Vector<LuminProbe*> timeSorted;
    for (auto& probe : frameProbes)
    {
        CHECK_CONTINUE(Config.UpdateFrequency < 0.0f && probe->Timestamp > 0.001f);
        CHECK_CONTINUE(Config.Iterations > 0 && probe->Iterations >= Config.Iterations);
        CHECK_CONTINUE(InArgs.ContextPtr->Time() - probe->Timestamp < Config.UpdateFrequency)
        Utility::SortedInsert(timeSorted, probe, [&](const LuminProbe* InFirst, const LuminProbe* InSecond)
        {
            // TODO: Also consider distance
            return InFirst->Timestamp < InSecond->Timestamp;
        });
    }

    CHECK_RETURN(timeSorted.empty(), {});

    Array<QuatF, 6> directions = RaylibRenderUtility::GetCubemapRotations();
    Vec2F size = Target.Size().To<float>();
    RenderArgs args = {
        .ScenePtr = InArgs.ScenePtr,
        .ContextPtr = &Context,
        .ViewportPtr = &Viewport,
        .LuminPtr = this,
        .LightsPtr = InArgs.LightsPtr,
        .Perspectives = {}
    };
    float range = GetRange();

    int count = 0;
    for (auto probe : timeSorted)
    {
        CHECK_CONTINUE(!probe);
        probe->Timestamp = Context.Time();
        probe->Iterations++;

        auto rect = AtlasMap.GetRect(probe->Coord.id, 0).To<float>();
        probe->Rect = {
            rect.x / size.x,
            rect.y / size.y,
            rect.z / size.x,
            rect.w / size.y
        };
        
        for (int i = 0; i < 6; i++)
        {
            args.Perspectives.push_back({
                .ReferenceRect = rect,
                .TargetRect = AtlasMap.GetRect(probe->Coord.id, i),
                .Camera = {
                    .Position = probe->Pos,
                    .Rotation = directions[i],
                    .FOV = 90.0f,
                    .Far = range * 2,
                    .Near = 0.1f
                }
            });
        }
        
        count++;
        if (count >= Config.MaxProbeRenders)
            break;
    }

    Viewport.BeginFrame();
    return Pipeline.RenderProbes(args, Config.CollectShader, Target);
}

Rendering::Pipeline::Stats Rendering::Lumin::LerpProbes(const RenderArgs& InArgs)
{
    RenderArgs lerpArgs = {
        .ScenePtr = InArgs.ScenePtr,
        .ContextPtr = &Context,
        .ViewportPtr = &Viewport,
        .LuminPtr = this,
        .LightsPtr = InArgs.LightsPtr,
        .Perspectives = {{
            .ReferenceRect= Vec4F(),
            .TargetRect= Vec4I(),
            .Camera= InArgs.ScenePtr->GetCamera()
        }}
    };
    return Pipeline.LerpProbes(lerpArgs, Config.LerpShader, Target, LerpTarget);
}

Vector<Rendering::LuminProbe*> Rendering::Lumin::GetProbes(const RenderArgs& InArgs)
{
    PROFILE_GL();
    
    CHECK_RETURN(InArgs.Perspectives.empty(), {})
    
    auto& cam = InArgs.Perspectives.at(0).Camera;
    Frustum frustum;
    frustum.ConstructFrustum(cam, Viewport.GetResolution());
    Vector<LuminProbe*> result;

    auto sortFunc = [&](const LuminProbe* InFirst, const LuminProbe* InSecond)
    {
        return (InFirst->Pos - cam.Position).LengthSqr() < (InSecond->Pos - cam.Position).LengthSqr();
    };

    auto checkFunc = [&](const LuminProbe& InProbe)
    {
        const Vec3F maxDist = (Vec3F(1.0f) / Config.Density.Get()) * Config.CullMultiplier.Get();
        const float cullDist = Utility::Math::Max(Utility::Math::Max(maxDist.x, maxDist.y), maxDist.z);
        if (frustum.CheckSphere(InProbe.Pos, cullDist))
            return true;
        return false;
    };
    
    for (auto& probe : Probes)
        if (checkFunc(probe.second))
            Utility::SortedInsert(result, &probe.second, sortFunc);

    auto count = Utility::Math::Min(static_cast<int>(result.size()), Config.MaxProbes.Get());
    CHECK_RETURN(count <= 0, {});
    return { result.begin(), result.begin() + count };
}

void Rendering::Lumin::ExpandVolume(const Scene& InScene)
{
    PROFILE_GL();
    
    for (auto& entry : InScene.Meshes.Entries)
    {
        for (auto& trans : entry.second.Transforms.GetAll())
        {
            // Get extent, maybe add coord?
            ProbeCoord coord = FromPos(trans.GetPosition());
            TryCreateProbe(coord);
        }
    }
}

void Rendering::Lumin::TryCreateProbe(const ProbeCoord InCoord)
{
    if (InCoord.id == 0)
        return;
    auto& probe = Probes[InCoord.id];
    if (probe.Coord.id == 0)
    {
        probe.Coord = InCoord;
        probe.Pos = FromCoord(InCoord);
    }
}

Rendering::ProbeCoord Rendering::Lumin::FromPos(const Vec3F& InPos)
{
    auto density = Config.Density.Get();
    return {
        .x = static_cast<int16>(InPos.x * density.x - 0.5f),
        .y = static_cast<int16>(InPos.y * density.y - 0.5f),
        .z = static_cast<int16>(InPos.z * density.z - 0.5f)
    };
}

Vec3F Rendering::Lumin::FromCoord(const ProbeCoord& InCoord)
{
    auto density = Config.Density.Get();
    return {                                
        (static_cast<float>(InCoord.x) + 0.5f) / density.x,
        (static_cast<float>(InCoord.y) + 0.5f) / density.y,
        (static_cast<float>(InCoord.z) + 0.5f) / density.z,
    };
}
