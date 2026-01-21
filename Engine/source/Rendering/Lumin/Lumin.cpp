#include "Lumin.h"

#include "Utility.h"
#include "Rendering/Utility.h"
#include "Scene/Scene.h"
#include "Scene/Culling/Frustum.h"
#include "Scene/Instances/CameraInstance.h"

void Rendering::Lumin::Init(const ContextConfig& InConfig)
{
    config = InConfig.Lumin;
    context.Init(InConfig);
    viewport.Init(config.Viewport.Get(), {});
    
    // Create storage for data chunks
    chunks.Init(config.ChunkAxisCells, config.CellSize);
}

void Rendering::Lumin::Deinit()
{
    viewport.Deinit();
    context.Deinit();
    debugProbes.clear();
    chunks.Deinit();
}

Rendering::Pipeline::Stats Rendering::Lumin::Update(const RenderArgs& InArgs)
{
    PROFILE_GL();
    
    CHECK_ASSERT(!InArgs.scenePtr, "Invalid scene");
    CHECK_ASSERT(!InArgs.viewportPtr, "Invalid viewport");

    if (auto tex = config.TexBRDF.Get().Get())
        if (!tex->IsBaked())
            tex->Bake();
    
    if (!config.Enabled)
        return {};
    
    Pipeline::Stats stats;
    // TODO: Handle fallback probe
    stats += UpdateProbes(InArgs);
    return stats;
}

Rendering::LuminData Rendering::Lumin::GetFrameData(const RenderArgs& InArgs) const
{
    Vec3F min;
    Vec3F max;
    Frustum frustum;
    GetFrustum(InArgs, frustum, min, max);
    return {
        chunks.GetFrameChunks(frustum, min, max),
        chunks.GetCellSize(),
        chunks.GetChunkSize()
    };
}

Vector<Mat4F> Rendering::Lumin::GetDebugProbes()
{
    if (!debugProbes.empty())
        return debugProbes;
    int size = config.ChunkAxisCells;
    debugProbes.reserve(size ^ 3);
    for (int z = 0; z < size; z++)
    for (int y = 0; y < size; y++)
    for (int x = 0; x < size; x++)
        debugProbes.push_back(Mat4F(Vec3I(x, y, z).To<float>()));
    return debugProbes;
}

Vector<Rendering::RenderTarget*> Rendering::Lumin::GetTargets()
{
    Vector<RenderTarget*> targets;
    for (auto& c : chunks.GetAll())
    {
        targets.push_back(&c.second->GetTargets03());
        targets.push_back(&c.second->GetTargets48());
    }
    return targets;
}

Rendering::Pipeline::Stats Rendering::Lumin::UpdateProbes(const RenderArgs& InArgs)
{
    PROFILE_GL();
    
    viewport.BeginFrame();
    
    Vec3F min;
    Vec3F max;
    Frustum frustum;
    GetFrustum(InArgs, frustum, min, max);
    
    Vec3F cell;
    LuminCoord coord;
    LuminChunkFrameData chunk;
    chunks.Expand(min, max);
    chunks.RefreshOldestProbe(min, max, chunk, cell, coord);
    CHECK_RETURN_LOG(!chunk.targets03 || !chunk.targets48, "No chunk", {});
    // TODO: Other args for the SH gather
    
    // Render probes to viewport framebuffer
    RenderArgs probeArgs = GetCubemapArgs(InArgs, cell);
    Pipeline::Stats stats = LuminPipeline::RenderProbes(probeArgs, false);
    
    // Collect sh coefficients
    RenderArgs shArgs = GetSHArgs(InArgs, cell, coord);
    stats += LuminPipeline::CollectSH(shArgs, chunk);
    return stats;
}

void Rendering::Lumin::GetFrustum(const RenderArgs& InArgs, Frustum& OutFrustum, Vec3F& OutMin, Vec3F& OutMax)
{
    auto size = InArgs.viewportPtr->GetSize().To<float>();
    auto cam = InArgs.scenePtr->GetCamera();
    
    // AABB
    OutMin = cam.position;
    OutMax = cam.position;
    for (const auto& c : cam.GetFrustumCorners(size))
    {
        if (c.x < OutMin.x) OutMin.x = c.x;
        if (c.y < OutMin.y) OutMin.y = c.y;
        if (c.z < OutMin.z) OutMin.z = c.z;
        if (c.x > OutMax.x) OutMax.x = c.x;
        if (c.y > OutMax.y) OutMax.y = c.y;
        if (c.z > OutMax.z) OutMax.z = c.z;
    }
    
    // Culling
    OutFrustum.ConstructFrustum(
        cam.GetProjectionMatrix(size), 
        cam.GetViewMatrix(), 
        cam.far, 
        cam.position);
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

Rendering::RenderArgs Rendering::Lumin::GetCubemapArgs(const RenderArgs &InArgs, const Vec3F& InProbePos)
{
    const Array<QuatF, 6> directions = GetCubemapRotations();
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
    
    Array<Vec3F, 9> points = GetCullPoints(InProbePos);
    args.cullPoints.insert(args.cullPoints.end(), points.begin(), points.end());
    for (int i = 0; i < 6; i++)
    {
        args.perspectives.push_back({
            .referenceRect = {},
            .targetRect = {},
            .camera = {
                .position = InProbePos,
                .rotation = directions[i],
                .fov = 90.0f,
                .far = config.Far,
                .near = config.Near
            },
            .layerFace = i
        });
    }
    
    return args;
}

Rendering::RenderArgs Rendering::Lumin::GetSHArgs(const RenderArgs &InArgs, const Vec3F& InCell, const LuminCoord& InCoord)
{
    // We are only rendering to 1 (one) pixel!
    // But 9 different textures... lol, optimization
    float axis = static_cast<float>(config.ChunkAxisCells.Get());
    Vec4F target = {
        InCoord.pos.x / axis,
        InCoord.pos.y / axis,
        (InCoord.pos.x + 1) / axis, 
        (InCoord.pos.y + 1) / axis, 
    };
    
    RenderArgs args = {
        .scenePtr = InArgs.scenePtr,
        .contextPtr = InArgs.contextPtr,
        .viewportPtr = &viewport, // SH viewport?
        .luminPtr = this,
        .lightsPtr = nullptr,
        .particlesPtr = nullptr,
        .perspectives = {{
            .referenceRect = {},
            .targetRect = target,
            .camera = {
                .position = InCell,
                .rotation = QuatF(),
                .fov = 90.0f,
                .far = config.Far,
                .near = config.Near
            },
            .layerFace = InCoord.pos.z
        }},
        .cullPoints = {},
        .cullMask = static_cast<uint8>(VisibilityMask::LUMIN)
    };
    return args;
}
