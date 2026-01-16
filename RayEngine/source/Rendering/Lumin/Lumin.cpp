#include "Lumin.h"

#include <ranges>

#include "raylib.h"
#include "RayRenderUtility.h"
#include "Scene/Scene.h"
#include "Scene/Culling/Frustum.h"
#include "Scene/Instances/CameraInstance.h"

void Rendering::Lumin::Init(const ContextConfig& InConfig)
{
    config = InConfig.Lumin;
    context.Init(InConfig);
    
    auto& viewConf = config.Viewport.Get(); 
    viewConf.Cubemap = true;
    auto& w = viewConf.Width.Get();
    w = Utility::Math::Min(w, 16);
    viewConf.Height = w;
    viewport.Init(viewConf, {});
    
    target.Setup(config.SHResolution.Get(), "TexFrameSH", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, RL_TEXTURE_FILTER_LINEAR, TexType::TEX_3D);
    for (auto& t : lerpTarget.All())
    {
        if (t.TryBeginSetup(config.SHResolution.Get()))
        {
            t.CreateBuffer("TexSH", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1, RL_TEXTURE_FILTER_POINT, TexType::TEX_3D);
            t.EndSetup();
        }
    }
}

void Rendering::Lumin::Deinit()
{
    timestamps.Clear();
    viewport.Deinit();
    context.Deinit();
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
    // stats += UpdateFallbackProbe(InArgs);
    if (config.Enabled)
    {
        stats += UpdateProbes(InArgs);
        stats += LerpProbes(InArgs);
    }
    return stats;
}

Rendering::LuminData Rendering::Lumin::GetFrameData()
{
    return {
        &lerpTarget.Curr(),
        config.ProbeSize
    };
}

Vector<Mat4F> Rendering::Lumin::GetDebugProbes(const RenderArgs& InArgs)
{
    auto size = InArgs.viewportPtr->GetSize().To<float>();
    auto cam = InArgs.scenePtr->GetCamera();
    
    // Culling
    Frustum frustum;
    frustum.ConstructFrustum(
        cam.GetProjectionMatrix(size), 
        cam.GetViewMatrix(), 
        cam.far, 
        cam.position);
    
    Vector<Mat4F> matrices;
    timestamps.TraverseRoot([&](const TimeGrid::RootNode& InRoot)
    {
        Vec3F rootPos = timestamps.GetRootPosition(InRoot);
        float extent = timestamps.GetDepthSize(0);
        if (!frustum.CheckCube(rootPos, extent))
            return;
    });
    
    // Probably simplify this somehow?
    // Quickly create grid of matrices
    
    return {};
}

Vec3F Rendering::Lumin::SelectProbe(const RenderArgs& InArgs)
{
    auto size = InArgs.viewportPtr->GetSize().To<float>();
    auto cam = InArgs.scenePtr->GetCamera();
    
    // Expand volume
    Vec3F min = cam.position;
    Vec3F max = cam.position;
    for (auto& c : cam.GetFrustumCorners(size))
    {
        if (min.x < c.x) min.x = c.x;
        if (min.y < c.y) min.y = c.y;
        if (min.z < c.z) min.z = c.z;
        if (max.x < c.x) max.x = c.x;
        if (max.y < c.y) max.y = c.y;
        if (max.z < c.z) max.z = c.z;
    }
    
    // Culling
    Frustum frustum;
    frustum.ConstructFrustum(
        cam.GetProjectionMatrix(size), 
        cam.GetViewMatrix(), 
        cam.far, 
        cam.position);
    
    TimeGrid::RootNode* smallestRoot = nullptr;;
    float smallestTime = 0;
    Vector<int> smallestStack;
    
    timestamps.Expand(min, max);
    timestamps.TraverseRoot([&](TimeGrid::RootNode& InRoot){
        
        Vec3F rootPos = timestamps.GetRootPosition(InRoot);
        float extent = timestamps.GetDepthSize(0);
        if (!frustum.CheckCube(rootPos, extent))
            return;
        
        // Traverse node children
        // Find smallest, test that
        Vector searchStack = { 0 };
        float searchTime = 99999999.f;
        while (searchStack.size() < TimeGrid::depth)
        {
            const int depth = static_cast<int>(searchStack.size() - 1);
            const int nodeIndex = searchStack.back();
            
            int searchIndex = -1;
            timestamps.TraverseChildren(InRoot, nodeIndex, depth, [&](const int InChildIndex) {
                timestamps.GetNode(InRoot, InChildIndex, depth, [&](float& InTimestamp) {
                    if (searchIndex >= 0 && InTimestamp > searchTime)
                        return;
                    // TODO: Maybe prioritize nodes closer to camera?
                    searchTime = InTimestamp;
                    searchIndex = InChildIndex;
                });
            });
            
            if (searchIndex == -1)
                break;
            searchStack.push_back(searchIndex);
        }
        
        if (searchTime < smallestTime)
        {
            smallestTime = searchTime;
            smallestStack = searchStack;
            smallestRoot = &InRoot; 
        }
    });
    
    if (smallestRoot)
    {
        Vec3F nodePosition = timestamps.GetRootPosition(*smallestRoot);
        
        // Set time for smallest node
        int backIndex = smallestStack.back();
        timestamps.GetNode(*smallestRoot, backIndex, TimeGrid::depth, [&](float& InTimestamp) {
            nodePosition += timestamps.GetNodeOffset(backIndex, TimeGrid::depth);
            InTimestamp = static_cast<float>(InArgs.contextPtr->Time());;
        });
        
        // Update the cached times
        for (int depth = static_cast<int>(smallestStack.size()) - 1; depth >= 0; depth--)
        {
            int nodeIndex = smallestStack[depth];
            
            // Iterate over all children
            // Find new smallest value
            float newSmallest = -1;
            timestamps.TraverseChildren(*smallestRoot, nodeIndex, depth, [&](const int InChild) {
                timestamps.GetNode(*smallestRoot, InChild, depth, [&](float& InTimestamp) {
                    if (newSmallest < 0 || InTimestamp < newSmallest)
                        newSmallest = InTimestamp;
                });
            });
            
            // Set that for this node
            // Also apply offset
            timestamps.GetNode(*smallestRoot, nodeIndex, depth - 1, [&](float& InTimestamp) {
                nodePosition += timestamps.GetNodeOffset(backIndex, TimeGrid::depth);
                InTimestamp = newSmallest;
            });
        }
        return nodePosition;
    }
    
    return Vec3F::Zero(); // Default value?
}

Rendering::Pipeline::Stats Rendering::Lumin::UpdateProbes(const RenderArgs& InArgs)
{
    PROFILE_GL();
    
    auto rect = atlas.GetRect(0, 0, true);
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
    
    for (int i = 0; i < config.FrameRenders; i++)
    {
        Vec3F probePos = SelectProbe(InArgs);
        Array<Vec3F, 9> points = GetCullPoints(probePos);
        args.cullPoints.insert(args.cullPoints.end(), points.begin(), points.end());
        for (int i = 0; i < 6; i++)
        {
            args.perspectives.push_back({
                .referenceRect = rect,
                .targetRect = atlas.GetRect(0, i, true),
                .camera = {
                    .position = probePos,
                    .rotation = directions[i],
                    .fov = 90.0f,
                    .far = config.Far,
                    .near = config.Near
                }
            });
        }
    }
    
    viewport.BeginFrame();
    
    // TODO: Render simplified fallback using same function
    return LuminPipeline::RenderProbes(args, config.CollectShader, target, false);
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