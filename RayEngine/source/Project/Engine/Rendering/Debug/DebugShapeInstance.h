#pragma once

namespace Rendering
{
    struct DebugShapeInstance
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
    
    struct DebugLineInstance
    {
        Vec3F Start;
        Vec3F End; 
        Color Color; 
    };
}
