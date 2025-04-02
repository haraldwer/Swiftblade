#include "Lumin.h"

#include "Scene/Instances/CameraInstance.h"
#include "Scene/Scene.h"
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

void Rendering::Lumin::UpdateProbes(const RenderArgs& InArgs)
{
    CHECK_ASSERT(!InArgs.Scene, "Invalid scene");
    CHECK_ASSERT(!InArgs.Viewport, "Invalid viewport");

    // TODO: Maybe clear lumin if static scene changed
    
    ExpandVolume(*InArgs.Scene, InArgs.Viewport->GetVirtualTarget());

    // Sort all probes
    // Do sorted insert

    // All probes share fbo. We cannot reuse the one from viewport because of blip logic.
    // All probes have their own texture, bound to fbo
    
    RenderArgs args = {
        .Scene = InArgs.Scene,
        .Context = &Context,
        .Viewport = &Viewport,
        .Lumin = this,
        .Camera = CameraInstance()
    };

    // TODO: Some directions should be rotated
    QuatF directions[6] = {
        QuatF::FromDirection(Vec3F::Right()),
        QuatF::FromDirection(Vec3F::Left()),
        QuatF::FromDirection(Vec3F::Up()),
        QuatF::FromDirection(Vec3F::Down()),
        QuatF::FromDirection(Vec3F::Forward()),
        QuatF::FromDirection(Vec3F::Backward())
    };

    for (auto& probe : Probes)
    {
        for (int i = 0; i < 6; i++)
        {
            args.Camera = CameraInstance {
                .Position = probe.second.Pos,
                .Rotation = directions[i],// Depends on index
                .FOV = 90.0f,
                .Far = 1000.0f,
                .Near = 0.01f
            };
            Pipeline.RenderProbeFace(args, probe.second.Target, i, Config.BlipShader);
        }
    }
}

void Rendering::Lumin::ExpandVolume(const Scene& InScene, const RenderTexture& InTex)
{
    for (auto& entry : InScene.Meshes.Entries)
    {
        for (auto& trans : entry.second.Transforms)
        {
            // Get extent, maybe add coord?
            Coord coord = FromPos(trans.GetPosition());
            auto& probe = Probes[coord.id];
            if (probe.Coord.id == 0 && coord.id != 0)
            {
                probe.Coord = coord;
                probe.Pos = FromCoord(coord);
                if (probe.Target.TryBeginSetup(InTex))
                {
                    probe.Target.CreateBuffer("EnvCube", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, true);
                    probe.Target.EndSetup(InTex);
                }
            }
        }
    }
}

Rendering::Lumin::Coord Rendering::Lumin::FromPos(const Vec3F& InPos)
{
    return {
        .x = static_cast<int16>(InPos.x / Config.Density),
        .y = static_cast<int16>(InPos.y / Config.Density),
        .z = static_cast<int16>(InPos.z / Config.Density)
    };
}

Vec3F Rendering::Lumin::FromCoord(const Coord& InCoord)
{
    return {
        static_cast<float>(InCoord.x) * Config.Density,
        static_cast<float>(InCoord.y) * Config.Density,
        static_cast<float>(InCoord.z) * Config.Density,
    };
}
