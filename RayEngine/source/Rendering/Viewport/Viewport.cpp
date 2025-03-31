#include "Viewport.h"

#include "ImGui/imgui.h"
#include "ImGui/rlImGui.h"

void Rendering::Viewport::Init(const ViewportConfig& InConfig)
{
    Config = InConfig;
    
    const float aspect = static_cast<float>(Config.Width) / static_cast<float>(Config.Height);
    const int virtualWidth = Config.RenderSize ?
        static_cast<int>(static_cast<float>(Config.RenderSize) * aspect) : Config.Width; 
    const int virtualHeight = Config.RenderSize ?
        Config.RenderSize : Config.Height;

    if (!VirtualTarget)
        VirtualTarget = new RenderTexture();
    
    if (VirtualTarget->texture.width == virtualWidth &&
        VirtualTarget->texture.height == virtualHeight)
        return;

    UnloadRenderTexture(*VirtualTarget);
    *VirtualTarget = LoadRenderTexture( virtualWidth, virtualHeight);
    Targets.Init(*VirtualTarget);
}

void Rendering::Viewport::Deinit()
{
    if (VirtualTarget != nullptr)
    {
        if (IsRenderTextureValid(*VirtualTarget))
            UnloadRenderTexture(*VirtualTarget);
        delete (VirtualTarget);
        VirtualTarget = nullptr;
    }
}

void Rendering::Viewport::Resize(const Vec2I& InSize)
{
    const Vec2I viewSize = GetSize();
    if (InSize == viewSize)
        return;
    Config.Width = InSize.x;
    Config.Height = InSize.y;
    Init(Config);
}

void Rendering::Viewport::BeginFrame()
{
    BeginTextureMode(*VirtualTarget);
    ClearBackground(::BLACK);

    // DT since last frame in this viewport
    Delta = DeltaTimer.Ellapsed();
    DeltaTimer = Utility::Timer();
}

void Rendering::Viewport::EndFrame()
{
    EndTextureMode();
    Position = Vec2F::Zero();
}

RenderTexture& Rendering::Viewport::GetVirtualTarget() const
{
    CHECK_ASSERT(!VirtualTarget, "Invalid target");
    return *VirtualTarget;
}

void Rendering::Viewport::ImDraw()
{
    rlImGuiImageRect(
        &VirtualTarget->texture,
        Config.Width,
        Config.Height,
        Rectangle{
            0, 0,
            static_cast<float>(VirtualTarget->texture.width),
            -static_cast<float>(VirtualTarget->texture.height)
        });

    const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    const ImVec2 windowPos = ImGui::GetWindowPos();
    Position = {
        vMin.x + windowPos.x,
        vMin.y + windowPos.y
    };
}

Vec2I Rendering::Viewport::GetResolution() const
{
    return {
        VirtualTarget->texture.width,
        VirtualTarget->texture.height
    };
}

Vec2I Rendering::Viewport::GetSize() const
{
    return {
        Config.Width,
        Config.Height
    };
}
