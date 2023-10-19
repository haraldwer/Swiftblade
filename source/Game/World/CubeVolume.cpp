#include "CubeVolume.h"

#include "Game/Game.h"
#include "Utility/Utility.h"

void CubeVolume::Init()
{
    // Load required meshes
    Model m = LoadModel("../content/test.obj");
    
    Volume v;
    v.SizeX = 3;
    v.SizeY = 2;
    v.SizeZ = 3;

    v.Types[Coord(0, 0, 0).Key] = 1; 
    v.Types[Coord(0, 0, 1).Key] = 1; 
    v.Types[Coord(1, 0, 0).Key] = 1;

    Apply(v);
}

void CubeVolume::Apply(const ::Volume& InVolume)
{
    const int sizeX = InVolume.SizeX - 1;
    const int sizeY = InVolume.SizeY - 1;
    const int sizeZ = InVolume.SizeZ - 1;
    
    const auto evalCoord = [&](int x, int y, int z)
    {
        const auto find = InVolume.Types.find(Coord(x, y, z).Key);
        if (find == InVolume.Types.end())
            return static_cast<uint8_t>(0);
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

void CubeVolume::Draw()
{
    //MeshInstance instance;
    //instance.Mesh = Model.meshes[0];
    //instance.Material = Model.materials[0];
    //
    //for (const auto& coord : Result)
    //{
    //    CHECK_CONTINUE(coord.second == 0);
    //    
    //    const Coord::Vec pos = Coord(coord.first).Pos;
    //    Transform t;
    //    t.translation =
    //        Vector3Add(Vector3(pos.X, pos.Y, pos.Z),
    //        Vector3(0.5f, 0.5f, 0.5f)); 
    //    t.rotation = QuaternionIdentity();
    //    t.scale = Vector3(0.5f, 0.5f, 0.5f);        
    //    instance.Transform = TransformToMatrix(t);
    //    
    //    Game::Get().GetRenderScene().AddMesh(instance); 
    //}
}
