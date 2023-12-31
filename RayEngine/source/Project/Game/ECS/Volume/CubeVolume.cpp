#include "CubeVolume.h"

#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Physics/Manager.h"
#include "Utility/Math/Grid.h"

void ECS::CubeVolume::Serialize(SerializeObj& InOutObj) const
{
    Component::Serialize(InOutObj);
    
    Map<uint8, Vector<uint32>> entries;
    for (const auto& entry : Data)
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

bool ECS::CubeVolume::Deserialize(const DeserializeObj& InObj)
{
    const bool result = Component::Deserialize(InObj);

    // Unpack
    String data;
    Utility::Deserialize(InObj, "Data", data);

    size_t index = 0; 
    while (true)
    {
        // Find ;
        const size_t find = data.find(';', index);
        if (find == std::string::npos)
            break; // Has reached end!

        // Split on ; 
        const String findStr = data.substr(index, find - index);

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
            Data[entry] = value;

            // Set bounds!
            const Coord coord = entry;
            MinBounds.x = MIN(coord.Pos.X, MinBounds.x);
            MinBounds.y = MIN(coord.Pos.Y, MinBounds.y);
            MinBounds.z = MIN(coord.Pos.Z, MinBounds.z);
            MaxBounds.x = MAX(coord.Pos.X, MaxBounds.x);
            MaxBounds.y = MAX(coord.Pos.Y, MaxBounds.y);
            MaxBounds.z = MAX(coord.Pos.Z, MaxBounds.z);
            
            valueIndex = entryFind + 1; 
        }
        
        index = find + 1; 
    }

    return result; 
}

Vec3F ECS::CubeVolume::CoordToPos(const Coord InCoord, const Mat4F& InWorld) const
{
    const Mat4F mat = Vec3F(
        InCoord.Pos.X,
        InCoord.Pos.Y,
        InCoord.Pos.Z) *
            (Scale * 2.0f);

    // Transform to world space
    return (mat * InWorld).GetPosition(); 
}

Coord ECS::CubeVolume::PosToCoord(const Vec3F& InPos, const Mat4F& InWorld) const
{
    // Transform to local space
    Vec3F localP = (Mat4F(InPos) * Mat4F::GetFastInverse(InWorld)).GetPosition(); 
    
    Vec3F p = localP * (1.0f / (Scale * 2.0f));

    return Coord(
        static_cast<uint8>(p.x),
        static_cast<uint8>(p.y),
        static_cast<uint8>(p.z));
}

uint8 ECS::SysCubeVolume::GetVal(const EntityID InID, const Coord InCoord)
{
    auto& v = Get<CubeVolume>(InID);
    auto& data = v.Data;
    const auto find = data.find(InCoord.Key);
    if (find != data.end())
        return find->second;
    return 0; 
}

void ECS::SysCubeVolume::Set(const EntityID InID, const Coord InStart, const Coord InEnd, const uint8 InVal)
{
    auto& v = Get<CubeVolume>(InID);
    auto& data = v.Data;
    auto& minB = v.MinBounds;
    auto& maxB = v.MaxBounds;
    
    const uint8 startX = MIN(InStart.Pos.X, InEnd.Pos.X); 
    const uint8 startY = MIN(InStart.Pos.Y, InEnd.Pos.Y); 
    const uint8 startZ = MIN(InStart.Pos.Z, InEnd.Pos.Z);
    const uint8 endX = MAX(InStart.Pos.X, InEnd.Pos.X); 
    const uint8 endY = MAX(InStart.Pos.Y, InEnd.Pos.Y); 
    const uint8 endZ = MAX(InStart.Pos.Z, InEnd.Pos.Z);
    
    for (uint8 x = startX; x <= endX; x++)
    {
        for (uint8 y = startY; y <= endY; y++)
        {
            for (uint8 z = startZ; z <= endZ; z++)
            {
                if (InVal == 0)
                {
                    data.erase(Coord(x, y, z).Key);
                }
                else
                {
                    data[Coord(x, y, z).Key] = InVal;

                    // Set cached bound
                    minB.x = MIN(x, minB.x);
                    minB.y = MIN(y, minB.y);
                    minB.z = MIN(z, minB.z);
                    maxB.x = MAX(x, maxB.x);
                    maxB.y = MAX(y, maxB.y);
                    maxB.z = MAX(z, maxB.z);
                }
            }
        }
    }
}

Coord ECS::SysCubeVolume::Trace(EntityID InID, const Vec3F& InPos, const Vec3F& InDir, int32 InMaxDist)
{
    auto& v = Get<CubeVolume>(InID);
    const auto& volume = v.Data;

    // Convert pos
    Vec3F origin = InPos * (1.0f / (v.Scale * 2.0f));
    
    Coord last = 0;
    int count = 0; 
    for (const auto& intersect : GridIntersection(origin, InDir, v.MaxBounds))
    {
        const Coord c = Coord(intersect.x, intersect.y, intersect.z);
        const auto find = volume.find(c.Key);
        const bool block = find != volume.end() && find->second != 0; 
        if (block || count == InMaxDist)
            return c;
        last = c;
        count++; 
    }
    return last;
}

void ECS::SysCubeVolume::DrawEditVolume(EntityID InID, Coord InStart, Coord InEnd)
{
    auto& v = Get<CubeVolume>(InID);
    const Mat4F world = Get<Transform>(InID).World();

    const int startX = MIN(InStart.Pos.X, InEnd.Pos.X); 
    const int startY = MIN(InStart.Pos.Y, InEnd.Pos.Y); 
    const int startZ = MIN(InStart.Pos.Z, InEnd.Pos.Z);
    const int endX = MAX(InStart.Pos.X, InEnd.Pos.X); 
    const int endY = MAX(InStart.Pos.Y, InEnd.Pos.Y); 
    const int endZ = MAX(InStart.Pos.Z, InEnd.Pos.Z);

    Vector<Mat4F> matrices; 
    for (int x = startX; x <= endX; x++)   
        for (int y = startY; y <= endY; y++)   
            for (int z = startZ; z <= endZ; z++)
                matrices.emplace_back(v.CoordToPos(Coord(x, y, z), world));                
    DrawCubes(matrices, true);
}

void ECS::SysCubeVolume::Init(const EntityID InID, CubeVolume& InComponent)
{
    BlockMesh.Material = ResRM("Defaults/RM_Default.json");
    BlockMesh.Model = ResModel("Defaults/M_Cube.obj");

    EditMesh.Material = ResRM("Editor/RM_EditCube.json");
    EditMesh.Model = ResModel("Defaults/M_Cube.obj");
    
    if (Engine::InstanceBase::Get().IsEditor())
        return;
    
    // Add collision cubes
    const Mat4F world = Get<Transform>(InID).World();
    Vector<Vec3F> cubes;
    for (const auto entry : InComponent.Data)
    {
        CHECK_CONTINUE(entry.second == 0)
        cubes.emplace_back(InComponent.CoordToPos(entry.first, world));
    }
    Physics::Manager::Get().AddCubes(InID, cubes, InComponent.Scale.Get());

    // Dress the volume
    InComponent.Dresser.Get().Dress(InComponent, *this); 
}

void ECS::SysCubeVolume::Deinit(EntityID InID, CubeVolume& InComponent)
{
    if (Engine::InstanceBase::Get().IsEditor())
        return;
    Physics::Manager::Get().ClearCubes(InID); 
}

void ECS::SysCubeVolume::Update(EntityID InID, CubeVolume& InComponent, double InDelta)
{
    // Add  render cubes
    const Mat4F world = Get<Transform>(InID).World();
    Vector<Mat4F> transforms;
    transforms.reserve(InComponent.Data.size() + 1);
    for (const auto& entry : InComponent.Data)
    {
        CHECK_CONTINUE(entry.second == 0);
        transforms.emplace_back(InComponent.CoordToPos(entry.first, world));
    }
    DrawCubes(transforms, false);

    // Draw dress
    InComponent.Dresser.Get().Draw();
}
void ECS::SysCubeVolume::DrawCubes(const Vector<Mat4F>& InTransforms, bool InEditMesh) const
{
    Engine::InstanceBase::Get().GetRenderScene().AddMeshes(
        InEditMesh ? EditMesh : BlockMesh,
        InTransforms);
}