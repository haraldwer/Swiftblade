#pragma once

namespace Rendering
{
    class AtlasMap
    {
    public:
        void Init(const Vec2I& InResolution, int InSlots, bool InCubemap);
        void Deinit();
        
        // Map ids to coordinates on the map
        Vec4I GetRect(uint32 InID, int InFace = 0);

    private:

        Vec4I CalcRect(int InIndex) const;
        
        struct Face
        {
            double AccessTime;
            int Index = -1;
        };
        
        Map<uint32, Face> Slots;
        Vector<int> Available; 

        int SlotResolution = 0;
        int AxisSlots = 0;
        bool Cubemap = false;
        Utility::Timer Timer;
    };
}

