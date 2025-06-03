#pragma once

typedef Map<uint32, uint8> CubeVolumeData;

namespace ECS
{
    struct CubeVolume;
}

struct Coord
{
    Coord()
    {
    }
    
    Coord(const uint8 InX, const uint8 InY, const uint8 InZ)
    {
        pos.x = InX;
        pos.y = InY;
        pos.z = InZ;
        pos.padding = 0; 
    }

    Coord(const uint32 InKey) : key(InKey) {}

    struct Vec
    {
        uint8 x;
        uint8 y;
        uint8 z;
        uint8 padding; 
    };
        
    union
    {
        Vec pos;
        uint32 key = 0; 
    };
};
