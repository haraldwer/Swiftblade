#include "CubeVolume.h"

#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Physics/Manager.h"
#include "Utility/Math/Geometry/GridIntersection.h"

void ECS::CubeVolume::UpdateCache(const Mat4F& InWorld)
{
    cachedMaxBounds = Utility::Math::Vector3<uint8>(0);
    cachedMinBounds = Utility::Math::Vector3<uint8>(-1);
    
    cachedCubeTransforms.clear();
    cachedCubeTransforms.reserve(data.size());
    for (const auto& entry : data)
    {
        CHECK_CONTINUE(entry.second == 0);
        const Coord coord = Coord(entry.first);
        cachedCubeTransforms.emplace_back(CoordToPos(entry.first, InWorld));
        cachedMinBounds.x = Utility::Math::Min(coord.pos.x, cachedMinBounds.x);
        cachedMinBounds.y = Utility::Math::Min(coord.pos.y, cachedMinBounds.y);
        cachedMinBounds.z = Utility::Math::Min(coord.pos.z, cachedMinBounds.z);
        cachedMaxBounds.x = Utility::Math::Max(coord.pos.x, cachedMaxBounds.x);
        cachedMaxBounds.y = Utility::Math::Max(coord.pos.y, cachedMaxBounds.y);
        cachedMaxBounds.z = Utility::Math::Max(coord.pos.z, cachedMaxBounds.z);
    }
}

void ECS::CubeVolume::CustomSerialize(SerializeObj& InOutObj) const
{   
    Map<uint8, Vector<uint32>> entries;
    for (const auto& entry : data)
    {
        CHECK_CONTINUE(!entry.second)
        entries[entry.second].push_back(entry.first); 
    }

    String result;
    for (const auto& value : entries)
    {
        result += std::to_string(value.first) + ":";
        for (const auto& entry : value.second)
            result += std::to_string(entry) + ",";
        result += ";";
    }

    Utility::Serialize(InOutObj, "Data", result); 
}

bool ECS::CubeVolume::CustomDeserialize(const DeserializeObj& InObj)
{
    // Unpack
    String stringData;
    Utility::Deserialize(InObj, "Data", stringData);

    size_t index = 0; 
    while (true)
    {
        // Find ;
        const size_t find = stringData.find(';', index);
        if (find == std::string::npos)
            break; // Has reached end!

        // Split on ; 
        const String findStr = stringData.substr(index, find - index);

        // Find :
        const size_t split = findStr.find(':');
        if (split == std::string::npos)
            break; // Has reached end!

        // Split on :
        const String valueStr = findStr.substr(0, split);
        const uint8 value = static_cast<uint8>(std::stoi(valueStr));
        const String entriesStr = findStr.substr(split + 1);

        // Find all values
        size_t valueIndex = 0; 
        while (true)
        {
            // Find ,
            const size_t entryFind = entriesStr.find(',', valueIndex);
            if (entryFind == std::string::npos)
                break; // Has reached end of entries

            // Split on ,
            const String entryStr = entriesStr.substr(valueIndex, entryFind - valueIndex);
            const uint32 entry = std::stoi(entryStr);

            // Add to data!
            data[entry] = value;
            valueIndex = entryFind + 1; 
        }
        
        index = find + 1; 
    }

    return true; 
}

Vec3F ECS::CubeVolume::CoordToPos(const Coord InCoord, const Mat4F& InWorld) const
{
    const Mat4F mat = Vec3F(
        InCoord.pos.x,
        InCoord.pos.y,
        InCoord.pos.z) *
            (Scale * 2.0f);

    // Transform to world space
    return (mat * InWorld).GetPosition(); 
}

Coord ECS::CubeVolume::PosToCoord(const Vec3F& InPos, const Mat4F& InWorld) const
{
    // Transform to local space
    const Vec3F localP = (Mat4F(InPos) * Mat4F::GetFastInverse(InWorld)).GetPosition();
    const Vec3F p = localP * (1.0f / (Scale * 2.0f));
    return {
        static_cast<uint8>(round(p.x)),
        static_cast<uint8>(round(p.y)),
        static_cast<uint8>(round(p.z))
    };
}

Vec3F ECS::CubeVolume::GetCenter(const bool InStart) const
{
    return CoordToPos(Coord(255 / 2, 255 / 2, (255 / 2) * static_cast<uint8>(!InStart)));
}

Coord ECS::CubeVolume::TryOffset(Coord InCoord, Vec3I InOffset)
{
    Vec3I pos = {
        InCoord.pos.x,
        InCoord.pos.y,
        InCoord.pos.z
    };
    Vec3I newPos = pos + InOffset;
    if (newPos.x < 0 ||
        newPos.y < 0 ||
        newPos.z < 0 ||
        newPos.x > static_cast<uint8>(-1) ||
        newPos.y > static_cast<uint8>(-1) ||
        newPos.z > static_cast<uint8>(-1))
        return Coord(0);
    return Coord(
        static_cast<uint8>(newPos.x),
        static_cast<uint8>(newPos.y),
        static_cast<uint8>(newPos.z));
}

Array<Coord, 6> ECS::CubeVolume::GetNeighbors(const Coord InCoord)
{
    Array<Coord, 6> result;
    result[0] = TryOffset(InCoord,{ 1, 0, 0 });
    result[1] = TryOffset(InCoord,{ -1, 0, 0 });
    result[2] = TryOffset(InCoord,{ 0, 1, 0 });
    result[3] = TryOffset(InCoord,{ 0, -1, 0 });
    result[4] = TryOffset(InCoord,{ 0, 0, 1 });
    result[5] = TryOffset(InCoord,{ 0, 0, -1 });
    return result;
}

uint8 ECS::SysCubeVolume::GetVal(const EntityID InID, const Coord InCoord)
{
    auto& v = Get<CubeVolume>(InID);
    auto& data = v.data;
    const auto find = data.find(InCoord.key);
    if (find != data.end())
        return find->second;
    return 0; 
}

void ECS::SysCubeVolume::Set(const EntityID InID, const Coord InStart, const Coord InEnd, const uint8 InVal)
{
    auto& v = Get<CubeVolume>(InID);
    auto& data = v.data;
    auto& minB = v.cachedMinBounds;
    auto& maxB = v.cachedMaxBounds;
    
    const uint8 startX = Utility::Math::Min(InStart.pos.x, InEnd.pos.x); 
    const uint8 startY = Utility::Math::Min(InStart.pos.y, InEnd.pos.y); 
    const uint8 startZ = Utility::Math::Min(InStart.pos.z, InEnd.pos.z);
    const uint8 endX = Utility::Math::Max(InStart.pos.x, InEnd.pos.x); 
    const uint8 endY = Utility::Math::Max(InStart.pos.y, InEnd.pos.y); 
    const uint8 endZ = Utility::Math::Max(InStart.pos.z, InEnd.pos.z);
    
    for (uint8 x = startX; x <= endX; x++)
    {
        for (uint8 y = startY; y <= endY; y++)
        {
            for (uint8 z = startZ; z <= endZ; z++)
            {
                if (InVal == 0)
                {
                    data.erase(Coord(x, y, z).key);
                }
                else
                {
                    data[Coord(x, y, z).key] = InVal;

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
    updateMesh = true;
}

Coord ECS::SysCubeVolume::Trace(const EntityID InID, const Vec3F& InPos, const Vec3F& InDir, const int32 InMaxDist)
{
    PROFILE();
    
    auto& v = Get<CubeVolume>(InID);
    const auto& volume = v.data;

    // Convert pos
    const Vec3F origin = InPos * (1.0f / (v.Scale.Get() * 2.0f));
    
    Coord last = 0;
    int count = 0; 
    for (const auto& intersect : GridIntersection(origin, InDir, InMaxDist * 1.5))
    {
        const uint8 max = -1; 
        if (intersect.x < 0 ||
            intersect.y < 0 ||
            intersect.z < 0 ||
            intersect.x >= max ||
            intersect.y >= max ||
            intersect.z >= max)
            continue; 
        const Coord c = Coord(
            static_cast<uint8>(intersect.x),
            static_cast<uint8>(intersect.y),
            static_cast<uint8>(intersect.z));
        const auto find = volume.find(c.key);
        const bool block = find != volume.end() && find->second != 0; 
        if (block || count == InMaxDist)
            return last.key != 0 ? last : c;
        last = c;
        count++;
    }
    return last;
}

void ECS::SysCubeVolume::DrawEditVolume(EntityID InID, Coord InStart, Coord InEnd)
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
                    v.CoordToPos(Coord(
                        static_cast<uint8>(x),
                        static_cast<uint8>(y),
                        static_cast<uint8>(z)), world),
                    QuatF::Identity(),
                    Vec3F(1.01f));

    Engine::Instance::Get().GetRenderScene().Meshes().Add(editMesh, matrices);
}

void ECS::SysCubeVolume::SystemInit()
{
    persistentID = MeshInstance::GenPersistentID();
    
    blockMesh = {
        .model = ResModel("Defaults/M_Cube.obj"),
        .material = ResRM("Dressing/RM_StoneWall.json"),
        .transform = {},
        .hash = 0
    };

    editMesh = {
        .model = ResModel("Defaults/M_Cube.obj"),
        .material = ResRM("Editor/RM_EditCube.json"),
        .transform = {},
        .hash = 0
    };
    
    // Generate hash
    blockMesh.hash = MeshInstance::GenHash(blockMesh.model, blockMesh.material);
    editMesh.hash = MeshInstance::GenHash(editMesh.model, editMesh.material);
}

void ECS::SysCubeVolume::Init(const EntityID InID, CubeVolume& InComponent)
{
    // Cache cube transforms
    const Mat4F world = Get<Transform>(InID).World();
    InComponent.UpdateCache(world);
    updateMesh = true;
    
    if (Engine::Instance::Get().IsEditor())
        return;
    
    // Add collision cubes
    Physics::Manager::Get().AddCubes(InID, InComponent.cachedCubeTransforms, InComponent.Scale.Get());
}

void ECS::SysCubeVolume::Deinit(EntityID InID, CubeVolume& InComponent)
{
    if (Engine::Instance::Get().IsEditor())
        return;
    Physics::Manager::Get().ClearCubes(InID); 
}

void ECS::SysCubeVolume::SystemFrame()
{
    CHECK_RETURN(!updateMesh);
    
    updateMesh = false;
    Rendering::MeshCollection& meshes = Engine::Instance::Get().GetRenderScene().Meshes();
    meshes.Remove(blockMesh.hash, persistentID);

    for (const auto& id : ComponentMap())
    {
        auto& c = GetInternal(id.first);
        meshes.Add(blockMesh, c.cachedCubeTransforms, persistentID);
    }
}
