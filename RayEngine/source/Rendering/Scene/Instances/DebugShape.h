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
        } type;

        Vec3F pos;
        QuatF rot; 
        Vec3F data;
        Color col; 
    };
    
    struct DebugLine
    {
        Vec3F start;
        Vec3F end; 
        Color col; 
    };
}
