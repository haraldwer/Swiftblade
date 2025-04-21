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
}

void Rendering::Lumin::Deinit()
{
    for (auto& p : Probes)
        p.second.Target.Unload();
    Probes.clear();
    Viewport.Deinit();
    Context.Deinit();
}

Vector<Rendering::LuminProbe*> Rendering::Lumin::GetProbes(const RenderArgs& InArgs)
{
    return {};
    /*
    Frustum frustum;
    frustum.ConstructFrustum(InArgs.Perspectives, Viewport.GetResolution());
    Vector<LuminProbe*> result;

    auto sortFunc = [&](const LuminProbe* InFirst, const LuminProbe* InSecond)
    {
        return (InFirst->Pos - InArgs.Perspectives.Position).LengthSqr() < (InSecond->Pos - InArgs.Perspectives.Position).LengthSqr();
    };

    float closeDist2 = Config.CloseCullDistance.Get() * Config.CloseCullDistance.Get();
    auto checkFunc = [&](const LuminProbe& InProbe)
    {
        const float probeCamDist = (InProbe.Pos - InArgs.Perspectives.Position).LengthSqr();
        if (probeCamDist < closeDist2)
            return true;
        const Vec3F maxDist = Vec3F(1.0f) / Config.Density.Get();
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
    */
}

Rendering::Pipeline::Stats Rendering::Lumin::Update(const RenderArgs& InArgs)
{
    return {};
    /*
    CHECK_ASSERT(!InArgs.Scene, "Invalid scene");
    CHECK_ASSERT(!InArgs.Viewport, "Invalid viewport");

    // TODO: Maybe clear lumin if static scene changed
    
    ExpandVolume(*InArgs.Scene);

    auto directions = RaylibRenderUtility::GetCubemapRotations();
    RenderArgs args = {
        .Scene = InArgs.Scene,
        .Context = &Context,
        .Viewport = &Viewport,
        .Lumin = this,
        .Perspectives = CameraInstance()
    };

    Vector<LuminProbe*> frameProbes = GetProbes(InArgs);
    Vector<LuminProbe*> timestampSorted;
    for (auto& probe : frameProbes)
    {
        CHECK_CONTINUE(Config.UpdateFrequency < 0.0f && probe->Timestamp != 0.0f);
        CHECK_CONTINUE(Config.Iterations > 0 && probe->Iterations >= Config.Iterations);
        CHECK_CONTINUE(InArgs.Context->Time() - probe->Timestamp < Config.UpdateFrequency)
        Utility::SortedInsert(timestampSorted, probe, [&](const LuminProbe* InFirst, const LuminProbe* InSecond)
        {
            // TODO: Also consider distance
            return InFirst->Timestamp < InSecond->Timestamp;
        });
    }

    Viewport.BeginFrame();
    Pipeline::Stats stats;
    int count = 0;
    for (auto probe : timestampSorted)
    {
        CHECK_CONTINUE(!probe);
        for (int i = 0; i < 6; i++)
        {
            args.Perspectives = CameraInstance {
                .Position = probe->Pos,
                .Rotation = directions[i],
                .FOV = 90.0f,
                .Far = Config.ProbeFar,
                .Near = 0.1f
            };
            stats += Pipeline.RenderProbeFace(args, probe->Target, Config.ProbeShader, i == 0);
            probe->Timestamp = Context.Time();
            probe->Iterations++;
        }
        count++;
        if (count >= Config.MaxProbeRenders)
            break;
    }
    return stats;
    */
}

void Rendering::Lumin::ExpandVolume(const Scene& InScene)
{
    for (auto& entry : InScene.Meshes.Entries)
    {
        for (auto& trans : entry.second.Transforms)
        {
            // Get extent, maybe add coord?
            ProbeCoord coord = FromPos(trans.GetPosition());
            for (int x = -1; x <= 1; x++)
                for (int y = -1; y <= 1; y++)
                    for (int z = -1; z <= 1; z++)
                        TryCreateProbe({
                            .x = static_cast<int16>(x + coord.x),
                            .y = static_cast<int16>(y + coord.y),
                            .z = static_cast<int16>(z + coord.z),
                            .layer = 0
                        });
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
        auto& tex = Viewport.GetVirtualTarget();
        if (probe.Target.TryBeginSetup(tex))
        {
            probe.Target.CreateBuffer(
                "TexEnvOct",
                PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
                1.0f);
            probe.Target.EndSetup(tex);
        }
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
