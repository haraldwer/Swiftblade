#include "CubeVolume.h"

#include "Engine/Instance/Instance.h"
#include "Utility/Math/Grid.h"

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
    
    Coord last = 0;
    int count = 0; 
    for (const auto& intersect : GridIntersection(InPos, InDir, bounds))
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

void ECS::SysCubeVolume::Init(EntityID InID, CubeVolume& InComponent)
{
    auto& data = InComponent.Data.Get();
    data[Coord(0, 0, 0).Key] = 1;
}

void ECS::SysCubeVolume::Update(EntityID InID, CubeVolume& InComponent, double InDelta)
{
    for (const auto& entry : InComponent.Data.Get())
    {
        CHECK_CONTINUE(entry.second == 0);
        const Coord coord = entry.first;
        DrawCube(Vec3F(
            coord.Pos.X,
            coord.Pos.Y,
            coord.Pos.Z)
            * InComponent.Scale.Get());
    }
    
    DrawCube(Vec3F(
        CachedTrace.Pos.X,
        CachedTrace.Pos.Y,
        CachedTrace.Pos.Z));
}
void ECS::SysCubeVolume::DrawCube(const Vec3F& InPos)
{
    MeshInstance m;
    m.Material = ResRM("Defaults/RM_Default.json");
    m.Model = ResModel("Defaults/M_Cube.obj");
    m.Transform = Mat4F(InPos, QuatF::Identity(), {0.5f});
    Engine::InstanceBase::Get().GetRenderScene().AddMesh(m);
}