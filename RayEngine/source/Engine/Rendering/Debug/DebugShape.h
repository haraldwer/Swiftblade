#pragma once

namespace Rendering
{
    struct DebugShape
    {
        enum class Type : uint8
        {
            SPHERE,
            BOX,
            CAPSULE
        } Type;

        Vec3F Pos;
        QuatF Rot; 
        Vec3F Data;
        Color Color; 
    };
}
