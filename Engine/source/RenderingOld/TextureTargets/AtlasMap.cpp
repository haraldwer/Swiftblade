#include "AtlasMap.h"

void Rendering::AtlasMap::Init(const int InSlots, const bool InCubemap, const int InResolution)
{
    int numSlots = InSlots;
    if (InCubemap)
        numSlots *= 6;
    axisSlots = static_cast<int>(sqrtf(static_cast<float>(numSlots))) + 1;
    CHECK_ASSERT(axisSlots * axisSlots < numSlots, "Invalid square root calculation")
    resolution = InResolution;

    float slotPart = 1.0f / axisSlots; 
    slotSize = slotPart * InResolution;
    cubemap = InCubemap;
    
    for (int i = 0; i < InSlots; i++)
        available.push_back(cubemap ? i * 6 : i);
    
    timer = {};
}

void Rendering::AtlasMap::Deinit()
{
    slots.clear();
    available.clear();
    persistentSlots.clear();
}

bool Rendering::AtlasMap::Contains(uint64 InID) const
{
    return slots.contains(InID);
}

Vec4F Rendering::AtlasMap::GetRect(uint64 InID, int InFace, bool InPersistent)
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
                    if (!persistentSlots.contains(s.first))
                    {
                        lowestTime = s.second.accessTime;
                        lowestTimeID = s.first;
                    }
                }
            }

            CHECK_ASSERT(lowestTimeID == static_cast<uint64>(-1), "Unable to find slot to free");
            available.push_back(slots.at(lowestTimeID).index);
            slots.erase(lowestTimeID);
        }

        slot.index = available.front();
        available.erase(available.begin());
        if (InPersistent)
            persistentSlots.insert(InID);
    }
    
    return CalcRect(slot.index + InFace);
}

Vec4F Rendering::AtlasMap::CalcRect(const int InIndex) const
{
    int hI = InIndex % axisSlots;
    int vI = (InIndex / axisSlots) % axisSlots;
    return (Vec4I({ hI, vI, 1, 1 }) * slotSize).To<float>() / resolution;
}
