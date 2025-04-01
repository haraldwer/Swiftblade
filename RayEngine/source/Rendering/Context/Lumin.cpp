#include "Lumin.h"

#include "Scene/Scene.h"

void Rendering::Lumin::Init()
{
    Config.LoadConfig();
    Context.Init(Config.Context);
    Viewport.Init(Config.Viewport);
}

void Rendering::Lumin::UpdateProbes(const Scene& InScene)
{
    ExpandVolume(InScene);
}

void Rendering::Lumin::Render(const RenderArgs& InArgs, const RenderTarget& InTarget)
{
}

void Rendering::Lumin::ExpandVolume(const Scene& InScene)
{
    for (auto& entry : InScene.Meshes.Entries)
    {
        for (auto& trans : entry.second.Transforms)
        {
            // Get extent, maybe add coord?
            Coord coord = FromPos(trans.GetPosition());
            auto& probe = Probes[coord.id];
            if (probe.Coord.id == 0)
            {
                probe.Coord = coord;
                probe.Pos = //??
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
