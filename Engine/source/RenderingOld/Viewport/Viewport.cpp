#include "Viewport.h"

void Rendering::Viewport::Init(const ViewportConfig &InConfig, const FXConfig &InFX)
{
    if (config == InConfig && fx == InFX)
        return;

    fx = InFX;
    config = InConfig;
    CHECK_ASSERT(config.Width <= 0 || config.Height <= 0, "Invalid viewport size");
    
    const float aspect = static_cast<float>(config.Width) / static_cast<float>(config.Height);
    const int virtualWidth = config.RenderSize ?
        static_cast<int>(static_cast<float>(config.RenderSize) * aspect) : config.Width; 
    const int virtualHeight = config.RenderSize ?
        config.RenderSize : config.Height;

    const Vec2I res = { virtualWidth, virtualHeight };
    targets.Init(res, fx, config.Cubemap);
}

void Rendering::Viewport::Deinit()
{
    targets.Deinit();
}

void Rendering::Viewport::Resize(const Vec2I& InSize)
{
    const Vec2I viewSize = GetSize();
    if (InSize == viewSize)
        return;
    config.Width = InSize.x;
    config.Height = InSize.y;
    Deinit();
    Init(config, fx);
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

Vec2F Rendering::Viewport::ScreenToViewport(const Vec2F &InScreenPos) const
{
    return InScreenPos - GetPosition();
}

Vec2F Rendering::Viewport::ScreenToViewportAbsolute(const Vec2F &InScreenPos) const
{
    const Vec2I si = GetSize();
    const Vec2F s = { static_cast<float>(si.x), static_cast<float>(si.y) };
    return ScreenToViewport(InScreenPos) / s;
}

Vec2F Rendering::Viewport::DistortCoord(const Vec2F &InAbsView) const
{
    constexpr float DISTORT_STRENGTH = 0.2f;
    constexpr float DISTORT_POW = 4.0f;
    constexpr float DISTORT_ZOOM = 0.15f;
    
    const Vec2F size = GetSize().To<float>();
    const float aspect = Utility::Math::Min(size.x, size.y) / Utility::Math::Max(size.x, size.y);
    const Vec2F pos = (InAbsView * 2 - 1) * Vec2F(1, aspect) * 0.75;
    const float dist = pos.Length() * (1 + DISTORT_ZOOM);

    const float p = std::pow(dist, DISTORT_POW);
    const float pow = 1 - Utility::Math::Clamp(p, 0.0f, 1.0f);
    const float zoom = 1 - pow * DISTORT_STRENGTH; // This is how much to zoom!

    const Vec2F newPos = (InAbsView * 2 - 1) * zoom;
    const Vec2F newCoord = (newPos + 1) / 2;
    return newCoord;
}

Vec2I Rendering::Viewport::GetResolution() const
{
    return targets.frameTargets.Curr().Size();
}

Vec2I Rendering::Viewport::GetSize() const
{
    return { config.Width, config.Height };
}
