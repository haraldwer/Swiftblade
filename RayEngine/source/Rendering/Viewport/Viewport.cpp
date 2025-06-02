#include "Viewport.h"

#include "raylib.h"
#include "ImGui/imgui.h"
#include "ImGui/rlImGui.h"

void Rendering::Viewport::Init(const ViewportConfig& InConfig)
{
    config = InConfig;

    CHECK_ASSERT(config.Width <= 0 || config.Height <= 0, "Invalid viewport size");
    
    const float aspect = static_cast<float>(config.Width) / static_cast<float>(config.Height);
    const int virtualWidth = config.RenderSize ?
        static_cast<int>(static_cast<float>(config.RenderSize) * aspect) : config.Width; 
    const int virtualHeight = config.RenderSize ?
        config.RenderSize : config.Height;

    if (!virtualTarget)
        virtualTarget = new RenderTexture();
    
    if (virtualTarget->texture.width == virtualWidth &&
        virtualTarget->texture.height == virtualHeight)
        return;

    UnloadRenderTexture(*virtualTarget);
    *virtualTarget = LoadRenderTexture( virtualWidth, virtualHeight);
    targets.Init(*virtualTarget);
}

void Rendering::Viewport::Deinit()
{
    if (virtualTarget != nullptr)
    {
        if (IsRenderTextureValid(*virtualTarget))
            UnloadRenderTexture(*virtualTarget);
        delete (virtualTarget);
        virtualTarget = nullptr;
    }
}

void Rendering::Viewport::Resize(const Vec2I& InSize)
{
    const Vec2I viewSize = GetSize();
    if (InSize == viewSize)
        return;
    config.Width = InSize.x;
    config.Height = InSize.y;
    Deinit();
    Init(config);
}

void Rendering::Viewport::BeginFrame()
{
    PROFILE_GL();
    // DT since last frame in this viewport
    delta = deltaTimer.Ellapsed();
    deltaTimer = Utility::Timer();
    viewProjPrev = viewProj;
}

void Rendering::Viewport::ResetPosition()
{
    position = Vec2F::Zero();
}

RenderTexture& Rendering::Viewport::GetVirtualTarget() const
{
    CHECK_ASSERT(!virtualTarget, "Invalid target");
    return *virtualTarget;
}

void Rendering::Viewport::ImDraw()
{
    rlImGuiImageRect(
        &virtualTarget->texture,
        config.Width,
        config.Height,
        Rectangle{
            0, 0,
            static_cast<float>(virtualTarget->texture.width),
            -static_cast<float>(virtualTarget->texture.height)
        });

    const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    const ImVec2 windowPos = ImGui::GetWindowPos();
    position = {
        vMin.x + windowPos.x,
        vMin.y + windowPos.y
    };
}

Vec2I Rendering::Viewport::GetResolution() const
{
    return {
        virtualTarget->texture.width,
        virtualTarget->texture.height
    };
}

Vec2I Rendering::Viewport::GetSize() const
{
    return {
        config.Width,
        config.Height
    };
}
