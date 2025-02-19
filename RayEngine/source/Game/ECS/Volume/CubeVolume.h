#pragma once

#include "Coord.h"
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"
#include "Engine/Rendering/Instances/MeshInstance.h"

namespace ECS
{
    struct CubeVolume : Component<CubeVolume>
    {
        PROPERTY_D(float, Scale, 1.0f); 

        // Manually serialized / deserialized
        CubeVolumeData Data;

        Utility::Math::Vector3<uint8> CachedMinBounds;
        Utility::Math::Vector3<uint8> CachedMaxBounds;
        Vector<Mat4F> CachedCubeTransforms;

        void UpdateCache(const Mat4F& InWorld);
        
        void CustomSerialize(SerializeObj& InOutObj) const override;
        bool CustomDeserialize(const DeserializeObj& InObj) override;
        
        Vec3F CoordToPos(Coord InCoord, const Mat4F& InWorld = Mat4F()) const;
        Coord PosToCoord(const Vec3F& InPos, const Mat4F& InWorld = Mat4F()) const;
        Vec3F GetCenter(bool InStart) const;

        static Coord TryOffset(Coord InCoord, Vec3I InOffset);
        static Array<Coord, 6> GetNeighbors(Coord InCoord);
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
        void Update(EntityID InID, CubeVolume& InComponent) override;

        bool ShouldUpdate() const override { return true; }
        int GetPriority() const override { return -101; }
        
    private:
        MeshInstance BlockMesh; 
        MeshInstance EditMesh;
    };
}

