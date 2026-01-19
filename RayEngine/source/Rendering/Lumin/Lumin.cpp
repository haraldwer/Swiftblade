#include "Lumin.h"

#include "RayRenderUtility.h"
#include "Scene/Scene.h"
#include "Scene/Culling/Frustum.h"
#include "Scene/Instances/CameraInstance.h"

void Rendering::Lumin::Init(const ContextConfig& InConfig)
{
    config = InConfig.Lumin;
    context.Init(InConfig);
    
    // Create viewport for cubemap rendering
    auto& viewConf = config.Viewport.Get(); 
    viewConf.Cubemap = true;
    auto& w = viewConf.Width.Get();
    w = Utility::Math::Min(w, 16);
    viewConf.Height = w;
    viewport.Init(viewConf, {});
    
    // Create storage for data chunks
    chunks.Init(config.ChunkAxisCells, config.CellSize);
}

void Rendering::Lumin::Deinit()
{
    viewport.Deinit();
    context.Deinit();
    debugProbes.clear();
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

Rendering::LuminData Rendering::Lumin::GetFrameData(const RenderArgs& InArgs)
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

Rendering::Pipeline::Stats Rendering::Lumin::UpdateProbes(const RenderArgs& InArgs)
{
    PROFILE_GL();
    
    viewport.BeginFrame();
    
    Vec3F min;
    Vec3F max;
    Frustum frustum;
    GetFrustum(InArgs, frustum, min, max);
    
    Vec3F cell;
    LuminChunkFrameData chunk;
    chunks.Expand(min, max);
    chunks.RefreshOldestProbe(min, max, chunk, cell);
    CHECK_ASSERT(!chunk.target, "No chunk");
    auto args = GetCubemapArgs(InArgs, cell);
    
    return LuminPipeline::RenderProbes(args, config.CollectShader, *chunk.target, false);
}

void Rendering::Lumin::GetFrustum(const RenderArgs& InArgs, Frustum& OutFrustum, Vec3F& OutMin, Vec3F& OutMax)
{
    auto size = InArgs.viewportPtr->GetSize().To<float>();
    auto cam = InArgs.scenePtr->GetCamera();
    
    // AABB
    OutMin = cam.position;
    OutMax = cam.position;
    for (auto& c : cam.GetFrustumCorners(size))
    {
        if (OutMin.x < c.x) OutMin.x = c.x;
        if (OutMin.y < c.y) OutMin.y = c.y;
        if (OutMin.z < c.z) OutMin.z = c.z;
        if (OutMax.x < c.x) OutMax.x = c.x;
        if (OutMax.y < c.y) OutMax.y = c.y;
        if (OutMax.z < c.z) OutMax.z = c.z;
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
            }
        });
    }
    
    return args;
}