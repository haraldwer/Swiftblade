﻿#pragma once

namespace Rendering
{
    struct Color
    {
        static Color Red() { return Color(255, 0, 0, 255); }
        static Color Green() { return Color(0, 255, 0, 255); }
        static Color Blue() { return Color(0, 0, 255, 255); }
        
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
