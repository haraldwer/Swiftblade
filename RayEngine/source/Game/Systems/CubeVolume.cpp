#include "CubeVolume.h"

#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Physics/Manager.h"
#include "Utility/Math/Grid.h"

Vec3F ECS::CubeVolume::CoordToPos(const Coord InCoord)
{
    return Vec3F(
        InCoord.Pos.X,
        InCoord.Pos.Y,
        InCoord.Pos.Z) *
            (Scale * 2.0f);
}

Coord ECS::CubeVolume::PosToCoord(const Vec3F& InPos)
{
    Vec3F p = InPos * (1.0f / (Scale * 2.0f));
    p.x = CLAMP(p.x, 0.0f, Width);
    p.y = CLAMP(p.y, 0.0f, Height);
    p.z = CLAMP(p.z, 0.0f, Depth);
    return Coord(
        static_cast<uint8>(p.x),
        static_cast<uint8>(p.y),
        static_cast<uint8>(p.z));
}

uint8 ECS::SysCubeVolume::GetVal(const EntityID InID, const Coord InCoord)
{
    auto& v = Get<CubeVolume>(InID);
    auto& data = v.Data.Get();
    const auto find = data.find(InCoord.Key);
    if (find != data.end())
        return find->second;
    return 0; 
}

void ECS::SysCubeVolume::Set(const EntityID InID, const Coord InStart, const Coord InEnd, const uint8 InVal)
{
    auto& v = Get<CubeVolume>(InID);
    auto& data = v.Data.Get();
    
    const int startX = MIN(InStart.Pos.X, InEnd.Pos.X); 
    const int startY = MIN(InStart.Pos.Y, InEnd.Pos.Y); 
    const int startZ = MIN(InStart.Pos.Z, InEnd.Pos.Z);
    const int endX = MAX(InStart.Pos.X, InEnd.Pos.X); 
    const int endY = MAX(InStart.Pos.Y, InEnd.Pos.Y); 
    const int endZ = MAX(InStart.Pos.Z, InEnd.Pos.Z);
    
    for (int x = startX; x <= endX; x++)   
        for (int y = startY; y <= endY; y++)   
            for (int z = startZ; z <= endZ; z++)
                InVal == 0 ?
                    data.erase(Coord(x, y, z).Key) :
                    data[Coord(x, y, z).Key] = InVal;
}

Coord ECS::SysCubeVolume::Trace(EntityID InID, const Vec3F& InPos, const Vec3F& InDir, int32 InMaxDist)
{
    auto& v = Get<CubeVolume>(InID);
    const auto& volume = v.Data.Get();
    const auto bounds = Utility::Math::Vector3<uint8>(
        v.Height, 
        v.Width,
        v.Depth);

    // Convert pos
    Vec3F origin = InPos * (1.0f / v.Scale);
    
    Coord last = 0;
    int count = 0; 
    for (const auto& intersect : GridIntersection(origin, InDir, bounds))
    {
        const Coord c = Coord(intersect.x, intersect.y, intersect.z);
        const auto find = volume.find(c.Key);
        const bool block = find != volume.end() && find->second != 0; 
        if (block || count == InMaxDist)
        {
            CachedTrace = c; 
            return c;
        }
        last = c;
        count++; 
    }
    CachedTrace = last;
    return last;
}

void ECS::SysCubeVolume::Init(const EntityID InID, CubeVolume& InComponent)
{
    MeshInstance.Material = ResRM("Defaults/RM_Default.json");
    MeshInstance.Model = ResModel("Defaults/M_Cube.obj");
    
    if (Engine::InstanceBase::Get().IsEditor())
        return;
    
    Vector<Vec3F> cubes;
    for (const auto entry : InComponent.Data.Get())
    {
        CHECK_CONTINUE(entry.second == 0)
        cubes.emplace_back(InComponent.CoordToPos(entry.first));
    }
    
    Physics::Manager::Get().AddCubes(InID, cubes, InComponent.Scale.Get()); 
}

void ECS::SysCubeVolume::Deinit(EntityID InID, CubeVolume& InComponent)
{
    if (Engine::InstanceBase::Get().IsEditor())
        return;
    Physics::Manager::Get().ClearCubes(InID); 
}

void ECS::SysCubeVolume::Update(EntityID InID, CubeVolume& InComponent, double InDelta)
{
    Mat4F world = Get<Transform>(InID).World();
    
    Vector<Mat4F> transforms;
    transforms.reserve(InComponent.Data.Get().size() + 1);

    // Add cubes
    for (const auto& entry : InComponent.Data.Get())
    {
        CHECK_CONTINUE(entry.second == 0);
        const Mat4F mat = Mat4F(InComponent.CoordToPos(entry.first));
        transforms.emplace_back(mat * world);
    }

    // Add trace cube
    if (Engine::InstanceBase::Get().IsEditor())
    {
        const Mat4F mat = Mat4F(
            InComponent.CoordToPos(CachedTrace));
        transforms.emplace_back(mat * world);
    }

    DrawCubes(transforms);
}
void ECS::SysCubeVolume::DrawCubes(const Vector<Mat4F>& InTransforms) const
{
    Engine::InstanceBase::Get().GetRenderScene().AddMeshes(MeshInstance, InTransforms);
}