#pragma once

namespace Utility
{
    // A helper struct where each coordinate corresponds to a unique ID
    // Useful for grid-based structures
    // Requirement: sizeof(KeyType) == (sizeof(CoordType) / 4)
    template <class KeyType = uint32, class CoordType = int8>
    struct Coord
    {
        Coord() { }
        Coord(const CoordType InX, const CoordType InY, const CoordType InZ, const CoordType InW = 0) : pos{ .x = InX, .y = InY, .z = InZ, .w = InW } { }
        Coord(const KeyType InKey) : key(InKey) {}

        struct CoordPos
        {
            CoordType x;
            CoordType y;
            CoordType z;
            CoordType w; 
        };
            
        union
        {
            CoordPos pos;
            KeyType key = 0; 
        };
    };
}
