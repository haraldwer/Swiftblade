#pragma once

namespace Rendering
{
    class AtlasMap
    {
    public:
        void Init(int InSlots, bool InCubemap);
        void Deinit();
        
        // Map ids to coordinates on the map
        bool Contains(uint64 InID) const;
        Vec4F GetRect(uint64 InID, int InFace = 0);
        float GetSlotSize() const { return slotSize; }

    private:

        Vec4F CalcRect(int InIndex) const;
        
        struct Face
        {
            double accessTime = 0.0;
            int index = -1;
        };
        
        Map<uint64, Face> slots = {};
        Vector<int> available = {}; 

        float slotSize = 0;
        int axisSlots = 0;
        bool cubemap = false;
        Utility::Timer timer = {};
    };
}

