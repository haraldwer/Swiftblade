#pragma once

#include "CubeVolumeData.h"
#include "Engine/ECS/Component.h"
#include "Engine/ECS/System.h"
#include "Rendering/Scene/Instances/MeshInstance.h"

namespace ECS
{
    struct CubeVolume : Component<CubeVolume>
    {
        PROPERTY_D(float, Scale, 1.0f);
        PROPERTY_D(ResRM, Material, "Dressing/RM_StoneWall.json");
        PROPERTY_D(ResRM, EditMaterial, "Editor/RM_EditBlock.json");
        PROPERTY_D(ResRM, MarkerMaterial, "Editor/RM_EditMarker.json");

        // Manually serialized / deserialized
        CubeVolumeData data;

        Utility::Math::Vector3<VolumeCoordValue> cachedMinBounds;
        Utility::Math::Vector3<VolumeCoordValue> cachedMaxBounds;
        Vector<Mat4F> cachedCubeTransforms;
        bool cacheUpdated = false;

        void UpdateCache(const Mat4F& InWorld);
        
        void CustomSerialize(SerializeObj& InOutObj) const override;
        bool CustomDeserialize(const DeserializeObj& InObj) override;
        
        Vec3F CoordToPos(VolumeCoord InCoord, const Mat4F& InWorld = Mat4F()) const;
        VolumeCoord PosToCoord(const Vec3F& InPos, const Mat4F& InWorld = Mat4F()) const;
        VolumeCoord GetCenter() const;

        static VolumeCoord TryOffset(VolumeCoord InCoord, Vec3I InOffset);
        static Array<VolumeCoord, 6> GetNeighbors(VolumeCoord InCoord);
        
        MeshInstance blockMesh = {}; 
        MeshInstance editMesh = {};
    };

    class SysCubeVolume : public System<CubeVolume>
    {
    public:
        uint8 GetVal(EntityID InID, VolumeCoord InCoord);
        void Set(EntityID InID, VolumeCoord InStart, VolumeCoord InEnd, uint8 InVal);
        VolumeCoord Trace(EntityID InID, const Vec3F& InPos, const Vec3F& InDir, int32 InMaxDist, VolumeCoord& OutHit);
        void DrawEditVolume(EntityID InID, VolumeCoord InStart, VolumeCoord InEnd);

        void SystemInit() override;
        void Init(EntityID InID, CubeVolume& InComponent) override;
        void Deinit(EntityID InID, CubeVolume& InComponent) override;
        void SystemFrame() override;

        bool ShouldUpdate() const override { return true; }
        int GetPriority() const override { return -101; }
        
    private:
        uint32 persistentID = 0;
    };
}

