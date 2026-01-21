#pragma once

namespace Rendering
{
    enum class VisibilityMask : uint8
    {
        NONE = 0,
        DEFAULT = 1 << 0,
        SHADOWS = 1 << 1,
        LUMIN = 1 << 2,
        ALL = 255,
    };
}
