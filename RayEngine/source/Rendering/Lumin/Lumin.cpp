#include "Lumin.h"

#include "Scene/Instances/CameraInstance.h"
#include "Scene/Scene.h"
#include "Utility/Collections/SortedInsert.h"
#include "raylib.h"

void Rendering::Lumin::Init(const ContextConfig& InDefaultContextConfig)
{
    Config.Context = InDefaultContextConfig;
    Config.LoadConfig(); // Will only overwrite, not reset
    Viewport.Init(Config.Viewport);
    Context.Init(Config.Context);
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
    Frustum frustum;
    frustum.ConstructFrustum(InArgs.Camera);
    Vector<LuminProbe*> result;

    auto sortFunc = [&](const LuminProbe* InFirst, const LuminProbe* InSecond)
    {
        return (InFirst->Pos - InArgs.Camera.Position).LengthSqr() < (InSecond->Pos - InArgs.Camera.Position).LengthSqr();
    };

    float closeDist2 = Config.CloseCullDistance.Get() * Config.CloseCullDistance.Get();
    auto checkFunc = [&](const LuminProbe& InProbe)
    {
        const float probeCamDist = (InProbe.Pos - InArgs.Camera.Position).LengthSqr();
        if (probeCamDist < closeDist2)
            return true;
        const Vec3F maxDist = Vec3F(1.0f) / Config.Density.Get();
        const float cullDist = Utility::Math::Max(Utility::Math::Max(maxDist.x, maxDist.y), maxDist.z);
        if (frustum.CheckSphere(InProbe.Pos, cullDist))
            return true;
        return true;
    };
    
    for (auto& probe : Probes)
        if (checkFunc(probe.second))
            Utility::SortedInsert(result, &probe.second, sortFunc);

    auto count = Utility::Math::Min(static_cast<int>(result.size()), Config.MaxProbes.Get());
    CHECK_RETURN(count <= 0, {});
    return { result.begin(), result.begin() + count }; 
}

Rendering::Pipeline::Stats Rendering::Lumin::UpdateProbes(const RenderArgs& InArgs)
{
    CHECK_ASSERT(!InArgs.Scene, "Invalid scene");
    CHECK_ASSERT(!InArgs.Viewport, "Invalid viewport");

    // TODO: Maybe clear lumin if static scene changed
    
    ExpandVolume(*InArgs.Scene);

    RenderArgs args = {
        .Scene = InArgs.Scene,
        .Context = &Context,
        .Viewport = &Viewport,
        .Lumin = this,
        .Camera = CameraInstance()
    };
    
    QuatF directions[6] = {
        // Yaw pitch roll
        QuatF::FromEuler({ 0, PI/2, PI }), //Right
        QuatF::FromEuler({ 0, -PI/2, -PI }), //Left
        QuatF::FromEuler({ PI/2, 0, PI }), //Up
        QuatF::FromEuler({ -PI/2, 0, PI }), //Down
        QuatF::FromEuler({ 0, 0, PI }), //Forward
        QuatF::FromEuler({ PI, 0, 0 }), //Backward
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
            args.Camera = CameraInstance {
                .Position = probe->Pos,
                .Rotation = directions[i],
                .FOV = 90.0f,
                .Far = Config.ProbeFar,
                .Near = 0.1f
            };
            stats += Pipeline.RenderProbeFace(args, probe->Target,  i, Config.ProbeShader);
            probe->Timestamp = Context.Time();
            probe->Iterations++;
        }
        //probe->Target.GenerateMips();
        count++;
        if (count >= Config.MaxProbeRenders)
            break;
    }
    Viewport.EndFrame();
    BeginTextureMode(InArgs.Viewport->GetVirtualTarget());
    
    return stats;
}

void Rendering::Lumin::ExpandVolume(const Scene& InScene)
{
    auto& tex = Viewport.GetVirtualTarget();
    for (auto& entry : InScene.Meshes.Entries)
    {
        for (auto& trans : entry.second.Transforms)
        {
            // Get extent, maybe add coord?
            ProbeCoord coord = FromPos(trans.GetPosition());
            for (int x = -1; x <= 1; x++)
                for (int y = -1; y <= 1; y++)
                    for (int z = -1; z <= 1; z++)
                        TryCreateProbe(tex, {
                            .x = static_cast<int16>(x + coord.x),
                            .y = static_cast<int16>(y + coord.y),
                            .z = static_cast<int16>(z + coord.z),
                            .layer = 0
                        });
        }
    }
}

void Rendering::Lumin::TryCreateProbe(const RenderTexture& InTex, const ProbeCoord InCoord)
{
    if (InCoord.id == 0)
        return;
    auto& probe = Probes[InCoord.id];
    if (probe.Coord.id == 0)
    {
        probe.Coord = InCoord;
        probe.Pos = FromCoord(InCoord);
        if (probe.Target.TryBeginSetup(InTex))
        {
            probe.Target.CreateBuffer("TexEnvCube", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1, true);
            probe.Target.EndSetup(InTex);
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
