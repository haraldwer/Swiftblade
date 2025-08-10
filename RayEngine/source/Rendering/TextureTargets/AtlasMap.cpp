#include "AtlasMap.h"

void Rendering::AtlasMap::Init(const int InSlots, const bool InCubemap)
{
    int numSlots = InSlots;
    if (InCubemap)
        numSlots *= 6;
    axisSlots = Utility::Math::SquareRoot(numSlots);
    slotSize = 1.0f / axisSlots;
    cubemap = InCubemap;
    
    for (int i = 0; i < InSlots; i++)
        available.push_back(cubemap ? i * 6 : i);
    
    timer = {};
}

void Rendering::AtlasMap::Deinit()
{
    slots.clear();
    available.clear();
}

Vec4F Rendering::AtlasMap::GetRect(const uint64 InID, const int InFace)
{
    // Track last access time
    auto& slot = slots[InID];
    slot.accessTime = timer.Ellapsed();
    if (slot.index == -1)
    {
        if (available.empty())
        {
            // Free oldest access slot
            uint64 lowestTimeID = static_cast<uint64>(-1);
            double lowestTime = -1.0f;
            for (auto& s : slots)
            {
                if (s.second.accessTime < lowestTime || lowestTime < 0.0)
                {
                    lowestTime = s.second.accessTime;
                    lowestTimeID = s.first;
                }
            }

            CHECK_ASSERT(lowestTimeID == static_cast<uint64>(-1), "Unable to find slot to free");
            available.push_back(slots.at(lowestTimeID).index);
            slots.erase(lowestTimeID);
        }

        slot.index = available.front();
        available.erase(available.begin());
    }
    
    return CalcRect(slot.index + InFace);
}

Vec4F Rendering::AtlasMap::CalcRect(const int InIndex) const
{
    int hI = InIndex % axisSlots;
    int vI = InIndex / axisSlots;
    return Vec4I({ hI, vI, 1, 1 }).To<float>() * slotSize;
}
