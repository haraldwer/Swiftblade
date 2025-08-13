#include "PropRuleVerticalCorner.h"

#include "ECS/Volume/CubeVolume.h"

bool PropRuleVerticalCorner::Evaluate(const ECS::CubeVolume& InVolume, ECS::VolumeCoord InCoord)
{
    return false;
    
    if (!InVolume.data.data.contains(InCoord.key))
        return false;
    
    // 0right 1left 2up 3down 4forward 5back
    auto neighbors = ECS::CubeVolume::GetNeighbors(InCoord);

    // Is up clear? 
    if (neighbors[2].key != 0)
        if (InVolume.data.data.contains(neighbors[2].key))
            return false;

    // Test every direction
    bool success = false;
    auto testDir = [&](int InIndex, Vec3F InDir)
    {
        if (neighbors[InIndex].key != 0)
        {
            if (!InVolume.data.data.contains(neighbors[InIndex].key))
            {
                Vec3F pos = InVolume.CoordToPos(InCoord);
                Mat4F trans = Mat4F(
                    pos + InDir + Vec3F::Up(),
                    QuatF::FromDirection(InDir),
                    Vec3F(1.0f, 0.3f, 0.3f));
                AddProp(trans, ledgeBP);
                success = true;
            }
        }
    };

    testDir(0, Vec3F::Right());
    testDir(1, Vec3F::Right() * -1.0f);
    testDir(4, Vec3F::Forward());
    testDir(5, Vec3F::Forward() * -1.0f);
    return success;
}
