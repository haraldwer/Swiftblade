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
        Pos.X = InX;
        Pos.Y = InY;
        Pos.Z = InZ;
        Pos.Padding = 0; 
    }

    Coord(const uint32 InKey) : Key(InKey) {}

    struct Vec
    {
        uint8 X;
        uint8 Y;
        uint8 Z;
        uint8 Padding; 
    };
        
    union
    {
        Vec Pos;
        uint32 Key = 0; 
    };
};
