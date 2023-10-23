#include "CubeVolume.h"

#include "Engine/Resource/ResourceManager.h"
#include "Game/Game.h"
#include "Utility/Utility.h"

void CubeVolume::Init()
{
    // Load required meshes
    
    Volume v;
    v.SizeX = 3;
    v.SizeY = 2;
    v.SizeZ = 3;

    v.Types[Coord(0, 0, 0).Key] = 1; 
    v.Types[Coord(0, 0, 1).Key] = 1; 
    v.Types[Coord(1, 0, 0).Key] = 1;

    Apply(v);
}

void CubeVolume::Apply(const Volume& InVolume)
{
    const int sizeX = InVolume.SizeX - 1;
    const int sizeY = InVolume.SizeY - 1;
    const int sizeZ = InVolume.SizeZ - 1;
    
    const auto evalCoord = [&](int x, int y, int z)
    {
        const auto find = InVolume.Types.find(Coord(x, y, z).Key);
        if (find == InVolume.Types.end())
            return static_cast<uint8>(0);
        return find->second;
    };
    
    const auto func = [&](int x, int y, int z)
    {
        // Check coord with offset
        // Does coord exist in volume?

        const bool x1y1 = evalCoord(x, y, z) > 0; 
        const bool x1y2 = evalCoord(x, y, z + 1) > 0; 
        const bool x2y1 = evalCoord(x + 1, y, z) > 0; 
        const bool x2y2 = evalCoord(x + 1, y, z + 1) > 0; 

        // Select variation depending on these
        // Allow rotation

        if (x1y1 || x1y2 || x2y1 || x2y2)
            Result[Coord(x, y, z).Key] = 1; 
    };
    
    for (int x = 0; x < sizeX; x++)
        for (int y = 0; y < sizeY; y++)
            for (int z = 0; z < sizeZ; z++)
                func(x, y, z);                
}

void CubeVolume::Draw() const
{
    for (const auto& coord : Result)
    {
        CHECK_CONTINUE(coord.second == 0);
        
        const Coord::Vec pos = Coord(coord.first).Pos;

        Rendering::Mesh m;
        m.Model = ResourceImpl::ResourceManager::Get().Request<Rendering::Model>("../content/test.obj");
        m.Transform = Mat4F(
            QuatF::Identity(),
            Vec3F::Zero(),
            Vec3F::Zero());
        
        Game::Get().GetRenderScene().AddMesh(m); 
    }
}
