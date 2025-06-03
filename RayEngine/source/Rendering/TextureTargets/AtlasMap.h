#pragma once

namespace Rendering
{
    class AtlasMap
    {
    public:
        void Init(const Vec2I& InResolution, int InSlots, bool InCubemap);
        void Deinit();
        
        // Map ids to coordinates on the map
        Vec4I GetRect(uint64 InID, int InFace = 0);

    private:

        Vec4I CalcRect(int InIndex) const;
        
        struct Face
        {
            double accessTime = 0.0;
            int index = -1;
        };
        
        Map<uint64, Face> slots = {};
        Vector<int> available = {}; 

        int slotResolution = 0;
        int axisSlots = 0;
        bool cubemap = false;
        Utility::Timer timer = {};
    };
}

