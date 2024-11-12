#include "PropRuleHorizontalCorner.h"

#include "Game/ECS/Volume/CubeVolume.h"

bool PropRuleHorizontalCorner::Evaluate(const ECS::CubeVolume& InVolume, Coord InCoord)
{
    if (!InVolume.Data.contains(InCoord.Key))
        return false;

    return false;
    
    // 0right 1left 2up 3down 4forward 5back
    auto neighbors = ECS::CubeVolume::GetNeighbors(InCoord);

    // Is up clear? 
    if (neighbors[2].Key != 0)
        if (InVolume.Data.contains(neighbors[2].Key))
            return false;

    // Test every direction
    bool success = false;
    auto testDir = [&](int InIndex, Vec3F InDir)
    {
        if (neighbors[InIndex].Key != 0)
        {
            if (!InVolume.Data.contains(neighbors[InIndex].Key))
            {
                Vec3F pos = InVolume.CoordToPos(InCoord);
                Mat4F trans = Mat4F(
                    pos + InDir + Vec3F::Up(),
                    QuatF::FromDirection(InDir),
                    Vec3F(1.0f, 0.3f, 0.3f));
                AddProp(trans, CornerBP);
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
