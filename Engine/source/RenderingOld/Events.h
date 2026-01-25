#pragma once

struct OnSetViewportSize
{
    Vec2F previousRenderResolution;
    Vec2F newRenderResolution;
    Vec2F previousViewportSize;
    Vec2F newViewportSize;
};

struct OnSetWindowSize
{
    Vec2F previousRenderResolution;
    Vec2F newRenderResolution;
    Vec2F previousViewportSize;
    Vec2F newViewportSize;
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
