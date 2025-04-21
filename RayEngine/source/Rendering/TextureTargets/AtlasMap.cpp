#include "AtlasMap.h"

void Rendering::AtlasMap::Init(const Vec2I& InResolution, int InSlots, bool InCubemap)
{
    if (InCubemap)
        InSlots *= 6;
    AxisSlots = Utility::Math::SquareRoot(InSlots);
    SlotResolution = InResolution.y / AxisSlots;
    Cubemap = InCubemap;

    for (int i = 0; i < InSlots; i++)
        Available.push_back(i);
    Timer = {};
}

void Rendering::AtlasMap::Deinit()
{
    Slots.clear();
    Available.clear();
}

Vec4I Rendering::AtlasMap::GetRect(const uint32 InID, const int InFace)
{
    // Cubemap means that each ID occupies multiple faces
    // Track last access time

    auto& slot = Slots[InID];
    slot.AccessTime = Timer.Ellapsed();
    if (slot.Index == -1)
    {
        if (Available.empty())
        {
            // Free oldest access slot
            uint32 LowestTimeID = static_cast<uint32>(-1);
            double LowestTime = 0.0;
            for (auto& s : Slots)
            {
                if (s.second.AccessTime < LowestTime || LowestTime == 0.0)
                {
                    LowestTime = s.second.AccessTime;
                    LowestTimeID = s.first;
                }
            }

            CHECK_ASSERT(LowestTimeID == static_cast<uint32>(-1), "Unable to find slot to free");
            Available.push_back(Slots.at(LowestTimeID).Index);
            Slots.erase(LowestTimeID);
        }

        slot.Index = Available.front();
        Available.erase(Available.begin());
    }
    
    return CalcRect(slot.Index + InFace);
}

Vec4I Rendering::AtlasMap::CalcRect(const int InIndex) const
{
    int hI = InIndex % AxisSlots;
    int vI = InIndex / AxisSlots;
    return Vec4I({ hI, vI, 1, 1 }) * SlotResolution;

    
}
