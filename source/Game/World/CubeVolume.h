#pragma once
#include <unordered_map>

struct Coord
{
    Coord(const uint8_t InX, const uint8_t InY, const uint8_t InZ)
    {
        Pos.X = InX;
        Pos.Y = InY;
        Pos.Z = InZ;
        Pos.Padding = 0; 
    }

    Coord(const uint32_t InKey) : Key(InKey) {}

    struct Vec
    {
        uint8_t X;
        uint8_t Y;
        uint8_t Z;
        uint8_t Padding; 
    };
        
    union
    {
        Vec Pos;
        uint32_t Key = 0; 
    };
};

struct Volume
{
    int SizeX = 0;
    int SizeY = 0;
    int SizeZ = 0;
    std::unordered_map<uint32_t, uint8_t> Types;
};

class CubeVolume
{
public:
    void Init();
    void Apply(const Volume& InVolume); 
    void Draw(); 
    
private:

    // Cached result
    std::unordered_map<uint32_t, uint8_t> Result;
    
};
