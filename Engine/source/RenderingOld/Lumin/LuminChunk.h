#pragma once
#include "Utility/Math/Coord.h"
#include "TextureTargets/RenderTarget.h"

namespace Rendering
{
    class Frustum;
    using LuminCoord = Utility::Coord<uint32, uint8>;

    class LuminChunk
    {
    public:
        
        void Init(int InCells);
        void Deinit();
        
        RenderTarget& GetTargets03() { return targets03; }
        RenderTarget& GetTargets48() { return targets48; }
        float GetOldestTimestamp() const;
        LuminCoord RefreshOldestCell();
        
    private:
        LuminCoord IndexToCoord(int InIndex) const;
        int CoordToIndex(const LuminCoord& InCoord) const;
        
        RenderTarget targets03 = {};
        RenderTarget targets48 = {};
        Vector<float> timestamps;
        Vector<int> timeIndices;
        int cells = 0;
    };
    
    struct LuminChunkFrameData
    {
        RenderTarget* targets03 = nullptr;
        RenderTarget* targets48 = nullptr;
        Vec3F position;
    };
    
    class LuminChunkCollection
    {
        using Coord = Utility::Coord<uint64, int16>;
    public:
        void Init(int InChunkAxisCells, const Vec3F& InCellSize);
        void Deinit();
        void Expand(const Vec3F& InMin, const Vec3F& InMax);

        void RefreshOldestProbe(const Vec3F &InMin, const Vec3F &InMax, LuminChunkFrameData &OutChunkData, Vec3F& OutCell, LuminCoord& OutCoord) const;
        
        Vector<LuminChunkFrameData> GetFrameChunks(const Frustum &InFrustum, const Vec3F &InMin, const Vec3F &InMax) const;
        Vec3F GetCellSize() const { return cellSize; }
        Vec3F GetChunkSize() const { return cellSize * chunkCells; }
        Map<uint32, LuminChunk*>& GetAll() { return chunks; }

    private:
        Coord PosToCoord(const Vec3F& InPos) const;
        
        Map<uint32, LuminChunk*> chunks;
        Vec3F cellSize;
        int chunkCells = 0;
    };
}
