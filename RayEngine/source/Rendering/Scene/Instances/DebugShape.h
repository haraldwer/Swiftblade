#pragma once

namespace Rendering
{
    struct Color
    {
        unsigned char r = 230;
        unsigned char g = 41;
        unsigned char b = 55;
        unsigned char a = 255;
    };

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
        Color Col; 
    };
    
    struct DebugLine
    {
        Vec3F Start;
        Vec3F End; 
        Color Col; 
    };
}
