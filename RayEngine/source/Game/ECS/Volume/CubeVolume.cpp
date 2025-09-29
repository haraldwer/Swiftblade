#include "CubeVolume.h"

#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Physics/Manager.h"
#include "Utility/Math/Geometry/GridIntersection.h"

void ECS::CubeVolume::UpdateCache(const Mat4F& InWorld)
{
    cachedMaxBounds = Utility::Math::Vector3<VolumeCoordValue>(0);
    cachedMinBounds = Utility::Math::Vector3<VolumeCoordValue>(0);

    bool set = false;
    cacheUpdated = true;
    cachedCubeTransforms.clear();
    cachedCubeTransforms.reserve(data.data.size());
    for (const auto& entry : data.data)
    {
        CHECK_CONTINUE(entry.second == 0);
        const VolumeCoord coord = VolumeCoord(entry.first);
        cachedCubeTransforms.emplace_back(CoordToPos(entry.first, InWorld));

        if (!set)
        {
            set = true;
            cachedMinBounds.x = coord.pos.x;
            cachedMinBounds.y = coord.pos.y;
            cachedMinBounds.z = coord.pos.z;
            cachedMaxBounds.x = coord.pos.x;
            cachedMaxBounds.y = coord.pos.y;
            cachedMaxBounds.z = coord.pos.z;
        }
        else
        {
            cachedMinBounds.x = Utility::Math::Min(coord.pos.x, cachedMinBounds.x);
            cachedMinBounds.y = Utility::Math::Min(coord.pos.y, cachedMinBounds.y);
            cachedMinBounds.z = Utility::Math::Min(coord.pos.z, cachedMinBounds.z);
            cachedMaxBounds.x = Utility::Math::Max(coord.pos.x, cachedMaxBounds.x);
            cachedMaxBounds.y = Utility::Math::Max(coord.pos.y, cachedMaxBounds.y);
            cachedMaxBounds.z = Utility::Math::Max(coord.pos.z, cachedMaxBounds.z);
        }
    }
}

void ECS::CubeVolume::CustomSerialize(SerializeObj &InOutObj) const
{
    data.Serialize(InOutObj);
}

bool ECS::CubeVolume::CustomDeserialize(const DeserializeObj &InObj)
{
    return data.Deserialize(InObj); 
}

Vec3F ECS::CubeVolume::CoordToPos(const VolumeCoord InCoord, const Mat4F& InWorld) const
{
    const VolumeCoord start = GetVolumeStart(); 
    const Mat4F mat = (
            Vec3F( InCoord.pos.x, InCoord.pos.y, InCoord.pos.z) -
            Vec3F( start.pos.x, start.pos.y, start.pos.z)) * 
            (Scale * 2.0f);

    // Transform to world space
    return (mat * InWorld).GetPosition(); 
}

ECS::VolumeCoord ECS::CubeVolume::PosToCoord(const Vec3F& InPos, const Mat4F& InWorld) const
{
    // Transform to local space
    const VolumeCoord start = GetVolumeStart();
    const Vec3F localP = (Mat4F(InPos) * Mat4F::GetFastInverse(InWorld)).GetPosition();
    const Vec3F p = localP * (1.0f / (Scale * 2.0f));
    return VolumeCoord(
        static_cast<VolumeCoordValue>(Utility::Math::Clamp(static_cast<int>(roundf(p.x)) + start.pos.x, INT8_MIN, INT8_MAX)),
        static_cast<VolumeCoordValue>(Utility::Math::Clamp(static_cast<int>(roundf(p.y)) + start.pos.y, INT8_MIN, INT8_MAX)),
        static_cast<VolumeCoordValue>(Utility::Math::Clamp(static_cast<int>(roundf(p.z)) + start.pos.z, INT8_MIN, INT8_MAX))
    );
}

ECS::VolumeCoord ECS::CubeVolume::GetVolumeStart()
{
    return VolumeCoord(0, 0, INT8_MIN);
}

ECS::VolumeCoord ECS::CubeVolume::TryOffset(VolumeCoord InCoord, Vec3I InOffset)
{
    Vec3I pos = {
        InCoord.pos.x,
        InCoord.pos.y,
        InCoord.pos.z
    };
    Vec3I newPos = pos + InOffset;
    if (newPos.x > INT8_MAX ||
        newPos.y > INT8_MAX ||
        newPos.z > INT8_MAX ||
        newPos.x < INT8_MIN ||
        newPos.y < INT8_MIN ||
        newPos.z < INT8_MIN)
        return { 0 };
    return {
        static_cast<VolumeCoordValue>(newPos.x),
        static_cast<VolumeCoordValue>(newPos.y),
        static_cast<VolumeCoordValue>(newPos.z)
    };
}

Array<ECS::VolumeCoord, 6> ECS::CubeVolume::GetNeighbors(const VolumeCoord InCoord)
{
    Array<VolumeCoord, 6> result;
    result[0] = TryOffset(InCoord,{ 1, 0, 0 });
    result[1] = TryOffset(InCoord,{ -1, 0, 0 });
    result[2] = TryOffset(InCoord,{ 0, 1, 0 });
    result[3] = TryOffset(InCoord,{ 0, -1, 0 });
    result[4] = TryOffset(InCoord,{ 0, 0, 1 });
    result[5] = TryOffset(InCoord,{ 0, 0, -1 });
    return result;
}

uint8 ECS::SysCubeVolume::GetVal(const EntityID InID, const VolumeCoord InCoord)
{
    auto& v = Get<CubeVolume>(InID);
    auto& volume = v.data;
    const auto find = volume.data.find(InCoord.key);
    if (find != volume.data.end())
        return find->second;
    return 0; 
}

void ECS::SysCubeVolume::Set(const EntityID InID, const VolumeCoord InStart, const VolumeCoord InEnd, const uint8 InVal)
{
    auto& v = Get<CubeVolume>(InID);
    auto& volume = v.data;
    auto& minB = v.cachedMinBounds;
    auto& maxB = v.cachedMaxBounds;
    
    const VolumeCoordValue startX = Utility::Math::Min(InStart.pos.x, InEnd.pos.x); 
    const VolumeCoordValue startY = Utility::Math::Min(InStart.pos.y, InEnd.pos.y); 
    const VolumeCoordValue startZ = Utility::Math::Min(InStart.pos.z, InEnd.pos.z);
    const VolumeCoordValue endX = Utility::Math::Max(InStart.pos.x, InEnd.pos.x); 
    const VolumeCoordValue endY = Utility::Math::Max(InStart.pos.y, InEnd.pos.y); 
    const VolumeCoordValue endZ = Utility::Math::Max(InStart.pos.z, InEnd.pos.z);
    
    for (VolumeCoordValue x = startX; x <= endX; x++)
    {
        for (VolumeCoordValue y = startY; y <= endY; y++)
        {
            for (VolumeCoordValue z = startZ; z <= endZ; z++)
            {
                if (InVal == 0)
                {
                    volume.data.erase(VolumeCoord(x, y, z).key);
                }
                else
                {
                    volume.data[VolumeCoord(x, y, z).key] = InVal;

                    // Set cached bound
                    minB.x = Utility::Math::Min(x, minB.x);
                    minB.y = Utility::Math::Min(y, minB.y);
                    minB.z = Utility::Math::Min(z, minB.z);
                    maxB.x = Utility::Math::Max(x, maxB.x);
                    maxB.y = Utility::Math::Max(y, maxB.y);
                    maxB.z = Utility::Math::Max(z, maxB.z);
                }
            }
        }
    }

    v.UpdateCache(Get<Transform>(InID).World());
}

ECS::VolumeCoord ECS::SysCubeVolume::Trace(const EntityID InID, const Vec3F& InPos, const Vec3F& InDir, const int32 InMaxDist, VolumeCoord& OutHit)
{
    PROFILE();
    
    auto& v = Get<CubeVolume>(InID);
    const auto& volume = v.data;

    // Convert pos to origin
    const VolumeCoord originCoord = v.PosToCoord(InPos);
    const Vec3F origin = {
        static_cast<float>(originCoord.pos.x),
        static_cast<float>(originCoord.pos.y),
        static_cast<float>(originCoord.pos.z),
    };
    
    OutHit = false;
    VolumeCoord last = 0;
    int count = 0;
    for (const Vec3I& intersect : GridIntersection(origin, InDir, InMaxDist * 1.5))
    {
        if (intersect.x < INT8_MIN ||
            intersect.y < INT8_MIN ||
            intersect.z < INT8_MIN ||
            intersect.x > INT8_MAX ||
            intersect.y > INT8_MAX ||
            intersect.z > INT8_MAX)
            continue; 
        const VolumeCoord c = VolumeCoord(
            static_cast<VolumeCoordValue>(intersect.x),
            static_cast<VolumeCoordValue>(intersect.y),
            static_cast<VolumeCoordValue>(intersect.z));
        const auto find = volume.data.find(c.key);
        const bool block = find != volume.data.end() && find->second != 0; 
        if (block || count == InMaxDist)
        {
            OutHit = block ? c : 0;
            return last.key != 0 ? last : c;
        }
        last = c;
        count++;
    }
    return last;
}

void ECS::SysCubeVolume::DrawEditVolume(EntityID InID, VolumeCoord InStart, VolumeCoord InEnd)
{
    auto& v = Get<CubeVolume>(InID);
    const Mat4F world = Get<Transform>(InID).World();

    const int startX = Utility::Math::Min(InStart.pos.x, InEnd.pos.x); 
    const int startY = Utility::Math::Min(InStart.pos.y, InEnd.pos.y); 
    const int startZ = Utility::Math::Min(InStart.pos.z, InEnd.pos.z);
    const int endX = Utility::Math::Max(InStart.pos.x, InEnd.pos.x); 
    const int endY = Utility::Math::Max(InStart.pos.y, InEnd.pos.y); 
    const int endZ = Utility::Math::Max(InStart.pos.z, InEnd.pos.z);

    Vector<Mat4F> matrices; 
    for (int x = startX; x <= endX; x++)   
        for (int y = startY; y <= endY; y++)   
            for (int z = startZ; z <= endZ; z++)
                matrices.emplace_back(
                    v.CoordToPos(VolumeCoord(
                        static_cast<VolumeCoordValue>(x),
                        static_cast<VolumeCoordValue>(y),
                        static_cast<VolumeCoordValue>(z)), world),
                    QuatF::Identity(),
                    Vec3F(1.01f));

    Engine::Instance::Get().GetRenderScene().Meshes().Add(v.editMesh, matrices);
}

void ECS::SysCubeVolume::SystemInit()
{
    persistentID = Rendering::MeshInstance::GenPersistentID();
}

void ECS::SysCubeVolume::Init(const EntityID InID, CubeVolume& InComponent)
{
    // Cache cube transforms
    const Mat4F world = Get<Transform>(InID).World();
    InComponent.UpdateCache(world);

    InComponent.blockMesh = {
        .model = ResModel("Defaults/M_Cube.obj"),
        .material = Engine::Instance::Get().IsEditor() ? InComponent.EditMaterial : InComponent.Material,
        .transform = {},
        .hash = 0,
        .mask = static_cast<uint8>(Rendering::MeshMask::ALL)
    };

    InComponent.editMesh = {
        .model = ResModel("Defaults/M_Cube.obj"),
        .material = InComponent.MarkerMaterial,
        .transform = {},
        .hash = 0,
        .mask = static_cast<uint8>(Rendering::MeshMask::DEFAULT)
    };
    
    // Generate hash
    InComponent.blockMesh.hash = Rendering::MeshInstance::GenHash(InComponent.blockMesh.model, InComponent.blockMesh.material);
    InComponent.editMesh.hash = Rendering::MeshInstance::GenHash(InComponent.editMesh.model, InComponent.editMesh.material);
    
    if (Engine::Instance::Get().IsEditor())
        return;
    
    // Add collision cubes
    Physics::Manager::Get().AddCubes(InID, InComponent.cachedCubeTransforms, InComponent.Scale.Get());
}

void ECS::SysCubeVolume::Deinit(EntityID InID, CubeVolume& InComponent)
{
    if (Engine::Instance::Get().IsEditor())
        return;
    Physics::Manager::Get().Remove(InID); 
}

void ECS::SysCubeVolume::SystemFrame()
{
    Rendering::MeshCollection& meshes = Engine::Instance::Get().GetRenderScene().Meshes();
    
    bool changed = false;
    for (const auto& id : ComponentMap())
        changed |= GetInternal(id.first).cacheUpdated;

    CHECK_RETURN(!changed);
    
    // First remove all
    for (const auto& id : ComponentMap())
    {
        auto& c = GetInternal(id.first);
        meshes.Remove(c.blockMesh.hash, persistentID);
    }

    // Then add all
    for (const auto& id : ComponentMap())
    {
        auto& c = GetInternal(id.first);
        meshes.Add(c.blockMesh, c.cachedCubeTransforms, persistentID);
        c.cacheUpdated = false;
    }

    // Splitting allows for one persistent ID => faster culling
}
