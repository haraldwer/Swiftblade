#include "CubeVolumeDresser.h"

#include "CubeVolume.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Rendering/Debug/Draw.h"

void CubeVolumeDresser::Dress(const ECS::CubeVolume& InVolume, const ECS::SysCubeVolume& InSys)
{
    // Get base transform
    const ECS::Transform* trans = ECS::Manager::Get().GetComponent<ECS::Transform>(InVolume.GetID());
    const Mat4F world = trans ? trans->World() : Mat4F();
    
    // Iterate over volume
    // With an offset, check all
    Map<uint8, Vector<Mat4F>> pending;
    for (uint8 x = InVolume.MinBounds.x; x < InVolume.MaxBounds.x; x++)
    {
        for (uint8 y = InVolume.MinBounds.y; y < InVolume.MaxBounds.y; y++)
        {
            for (uint8 z = InVolume.MinBounds.z; z < InVolume.MaxBounds.z; z++)
            {
                // For each position
                // Evaluate neighbours
                // TODO: Consider rotation!

                const auto getVal = [&](const uint8 xOff, const uint8 yOff, const uint8 zOff)
                {
                    const Coord pos = Coord(x + xOff, y + yOff, z + zOff);
                    const auto find = InVolume.Data.find(pos.Key);
                    if (find != InVolume.Data.end())
                        return static_cast<uint8>(find->second);
                    return static_cast<uint8>(0); 
                };

                // Get neighbors 
                Array<uint8, 8> neighbors;
                neighbors[0] = getVal(0, 0, 0); 
                neighbors[1] = getVal(1, 0, 0); 
                neighbors[2] = getVal(0, 0, 1); 
                neighbors[3] = getVal(1, 0, 1);
                neighbors[4] = getVal(0, 1, 0); 
                neighbors[5] = getVal(1, 1, 0); 
                neighbors[6] = getVal(0, 1, 1); 
                neighbors[7] = getVal(1, 1, 1);
                
                uint8 dress = EvaluateDress(neighbors);
                CHECK_CONTINUE(dress == 0);
                
                // Depending on neighbours, select config
                QuatF rot = QuatF::Identity();
                Vec3F pos = InVolume.CoordToPos(Coord(x, y, z), world) + Vec3F::One();
                pending[dress].emplace_back(pos, rot, Vec3F::One());
                LOG("Instance added");
            }
        }
    }
    
    // Get mesh data
    auto model = ResModel("Defaults/M_Cube.obj");
    auto material = ResRM("Defaults/RM_Default.json");

    Instances.clear();
    int num = 0;
    for (auto& p : pending)
    {
        auto& data = Instances.emplace_back();
        data.Instances = p.second;
        num += static_cast<int>(p.second.size());
        data.Mesh.Model = model;
        data.Mesh.Material = material;
    }
    LOG("Dressing complete, instances: " + std::to_string(num));
}

uint8 CubeVolumeDresser::EvaluateDress(const Array<uint8, 8>& InNeighbors) const
{

    const auto test = [](const Set<uint8>& InValid, const Array<uint8, 8>& InNeighbors)
    {
        for (uint8 i = 0; i < 8; i++)
            if (InValid.contains(i) != InNeighbors[i])
                return false;
        return true; 
    };

    
    // Flat wall
    if (test({0, 1, 4, 5}, InNeighbors))
        return 1;

    // Rotated right = x -> z && z -> -x
    
    return 0; 
}

void CubeVolumeDresser::Draw()
{
    for (auto& mesh : Instances)
    {
        //Engine::InstanceBase::Get().GetRenderScene().AddMeshes(
        //   mesh.Mesh,
        //   mesh.Instances);

        for (auto m : mesh.Instances)
            Rendering::DebugSphere(m.GetPosition(), 1.2f);
    } 
}
