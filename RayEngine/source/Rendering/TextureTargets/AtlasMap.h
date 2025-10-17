#pragma once

namespace Rendering
{
    class AtlasMap
    {
    public:
        void Init(int InSlots, bool InCubemap, int InResolution);
        void Deinit();
        
        // Map ids to coordinates on the map
        bool Contains(uint64 InID) const;
        Vec4F GetRect(uint64 InID, int InFace = 0, bool InPersistent = false);
        int GetSlotSize() const { return slotSize; }

    private:

        Vec4F CalcRect(int InIndex) const;
        
        struct Face
        {
            double accessTime = 0.0;
            int index = -1;
        };
        
        Map<uint64, Face> slots = {};
        Set<uint64> persistentSlots = {};
        Vector<int> available = {}; 

        int slotSize = 0;
        int axisSlots = 0;
        bool cubemap = false;
        int resolution = 0;
        Utility::Timer timer = {};
    };
}

