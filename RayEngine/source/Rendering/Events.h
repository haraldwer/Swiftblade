#pragma once

struct OnSetViewportSize
{
    Vec2F PreviousRenderResolution;
    Vec2F NewRenderResolution;
    Vec2F PreviousViewportSize;
    Vec2F NewViewportSize;
};

struct OnSetWindowSize
{
    Vec2F PreviousRenderResolution;
    Vec2F NewRenderResolution;
    Vec2F PreviousViewportSize;
    Vec2F NewViewportSize;
};

struct OnConfigChanged
{
};

struct OnBeginVirtualFrame
{
};

struct OnBeginFrame
{
};
