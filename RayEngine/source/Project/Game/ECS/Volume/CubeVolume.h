#pragma once

#include "VolumeDresser.h"
#include "Engine/ECS/System.h"
#include "Engine/ECS/Component.h"
#include "Engine/Rendering/Instances/MeshInstance.h"

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
    typedef Map<uint32, uint8> CubeVolumeData; 
    
    struct CubeVolume : Component<CubeVolume>
    {
        PROPERTY_D(float, Scale, 1.0f); 

        // Manually serialized / deserialized
        CubeVolumeData Data;

        Utility::Math::Vector3<uint8> CachedMinBounds;
        Utility::Math::Vector3<uint8> CachedMaxBounds;
        Vector<Mat4F> CachedCubeTransforms;

        void UpdateCache(const Mat4F& InWorld);
        
        void Serialize(SerializeObj& InOutObj) const override;
        bool Deserialize(const DeserializeObj& InObj) override;
        
        Vec3F CoordToPos(Coord InCoord, const Mat4F& InWorld) const;
        Coord PosToCoord(const Vec3F& InPos, const Mat4F& InWorld) const;
    };

    class SysCubeVolume : public System<CubeVolume>
    {
    public:
        uint8 GetVal(EntityID InID, Coord InCoord);
        void Set(EntityID InID, Coord InStart, Coord InEnd, uint8 InVal);
        Coord Trace(EntityID InID, const Vec3F& InPos, const Vec3F& InDir, int32 InMaxDist);
        void DrawEditVolume(EntityID InID, Coord InStart, Coord InEnd);
        
        void Init(EntityID InID, CubeVolume& InComponent) override;
        void Deinit(EntityID InID, CubeVolume& InComponent) override;
        void Frame(EntityID InID, CubeVolume& InComponent) override;
        void DrawCubes(const Vector<Mat4F>& InTransforms, bool InEditMesh) const;

        bool ShouldUpdate() const override { return true; }
        int GetPriority() const override { return -101; }
        
    private:
        MeshInstance BlockMesh; 
        MeshInstance EditMesh;
    };
}

