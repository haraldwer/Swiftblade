#pragma once

#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"

struct Coord
{
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

namespace ECS
{
    typedef Map<uint32, uint8> Volume; 
    
    struct CubeVolume : Component<CubeVolume>
    {
        PROPERTY_P(float, Scale, 1.0f);
        PROPERTY_P(uint8, Height, 50);
        PROPERTY_P(uint8, Width, 50);
        PROPERTY_P(uint8, Depth, 50);
        PROPERTY(Volume, Data);
    };

    class SysCubeVolume : public System<CubeVolume>
    {
    public:
        uint8 GetVal(EntityID InID, Coord InCoord);
        void Set(EntityID InID, Coord InStart, Coord InEnd, uint8 InVal);
        Coord Trace(EntityID InID, const Vec3F& InPos, const Vec3F& InDir, int32 InMaxDist);
        
        void Init(EntityID InID, CubeVolume& InComponent) override;
        void Update(EntityID InID, CubeVolume& InComponent, double InDelta) override;
        static void DrawCube(const Vec3F& InPos);

        bool ShouldUpdate() const override { return true; }
    private:
        Coord CachedTrace = 0; 
    };
}

