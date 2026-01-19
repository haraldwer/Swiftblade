#pragma once
#include "Math/Coord.h"
#include "TextureTargets/RenderTarget.h"

namespace Rendering
{
    class Frustum;

    class LuminChunk
    {
        using Coord = Utility::Coord<uint32, uint8>;
    public:
        
        void Init(int InCells);
        void Unload();
        
        RenderTarget& GetTarget() { return target; }
        float GetOldestTimestamp() const;
        Coord RefreshOldestCell();
        
    private:
        Coord IndexToCoord(int InIndex) const;
        int CoordToIndex(const Coord& InCoord) const;
        
        RenderTarget target = {};
        Vector<float> timestamps;
        Vector<int> timeIndices;
        int cells = 0;
    };
    
    struct LuminChunkFrameData
    {
        RenderTarget* target = nullptr;
        Vec3F position;
    };
    
    class LuminChunkCollection
    {
        using Coord = Utility::Coord<uint32, uint8>;
    public:
        void Init(int InChunkAxisCells, const Vec3F& InCellSize);
        void Unload();
        void Expand(const Vec3F& InMin, const Vec3F& InMax);

        void RefreshOldestProbe(const Vec3F &InMin, const Vec3F &InMax, LuminChunkFrameData &OutChunkData, Vec3F &OutCell) const;
        
        Vector<LuminChunkFrameData> GetFrameChunks(const Frustum &InFrustum, const Vec3F &InMin, const Vec3F &InMax) const;
        Vec3F GetCellSize() const { return cellSize; }
        Vec3F GetChunkSize() const { return cellSize * chunkCells; }
        
    private:
        Coord PosToCoord(const Vec3F& InPos) const;
        
        Map<uint32, LuminChunk*> chunks;
        Vec3F cellSize;
        int chunkCells = 0;
    };
}
