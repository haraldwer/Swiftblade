#include "LuminChunk.h"

#include <random>

#include "raylib.h"
#include "rlgl.h"

void Rendering::LuminChunk::Init(int InCells)
{
    cells = InCells;
    
    // 3D textures cannot be interpolated
    if (target.TryBeginSetup(InCells))
    {
        target.CreateBuffer("TexSH0", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1, RL_TEXTURE_FILTER_POINT, 0, TexType::TEX_3D);
        target.CreateBuffer("TexSH1", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1, RL_TEXTURE_FILTER_POINT, 0, TexType::TEX_3D);
        target.CreateBuffer("TexSH2", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1, RL_TEXTURE_FILTER_POINT, 0, TexType::TEX_3D);
        target.CreateBuffer("TexSH3", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1, RL_TEXTURE_FILTER_POINT, 0, TexType::TEX_3D);
        target.CreateBuffer("TexSH4", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1, RL_TEXTURE_FILTER_POINT, 0, TexType::TEX_3D);
        target.CreateBuffer("TexSH5", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1, RL_TEXTURE_FILTER_POINT, 0, TexType::TEX_3D);
        target.CreateBuffer("TexSH6", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1, RL_TEXTURE_FILTER_POINT, 0, TexType::TEX_3D);
        target.CreateBuffer("TexSH7", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1, RL_TEXTURE_FILTER_POINT, 0, TexType::TEX_3D);
        target.CreateBuffer("TexSH8", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1, RL_TEXTURE_FILTER_POINT, 0, TexType::TEX_3D);
        target.EndSetup();
    }
    
    int totalCells = InCells ^ 3;
    timestamps.resize(totalCells);
    timeIndices.reserve(totalCells);
    for (int i = 0; i < totalCells; i++)
        timeIndices.push_back(i);
    
    std::random_device rd;
    std::mt19937 rng(rd());
    std::ranges::shuffle(timeIndices, rng);
}

void Rendering::LuminChunk::Unload()
{
    target.Unload();
}

float Rendering::LuminChunk::GetOldestTimestamp() const
{
    return timestamps.at(timeIndices.at(0));
}

Rendering::LuminChunk::Coord Rendering::LuminChunk::RefreshOldestCell()
{
    // Convert coord to index
    int timestampIndex = timeIndices.at(0);
    
    // Move timestamp to back
    timeIndices.erase(timeIndices.begin());
    timeIndices.push_back(timestampIndex);
    
    timestamps[timestampIndex] = Utility::Time::Get().Unscaled();
    return IndexToCoord(timestampIndex);
}

Rendering::LuminChunk::Coord Rendering::LuminChunk::IndexToCoord(const int InIndex) const
{
    Coord c;
    c.pos.x = InIndex % cells;
    c.pos.y = InIndex / cells % cells;
    c.pos.z = InIndex / (cells * cells);
    return c;
}

int Rendering::LuminChunk::CoordToIndex(const Coord& InCoord) const
{
    return InCoord.pos.x
         + InCoord.pos.y * cells
         + InCoord.pos.z * cells * cells;
}

void Rendering::LuminChunkCollection::Init(int InChunkAxisCells, const Vec3F& InCellSize)
{
    cellSize = InCellSize;
    chunkCells = InChunkAxisCells;
}

void Rendering::LuminChunkCollection::Unload()
{
    for (auto& c : chunks)
    {
        c.second->Unload();
        delete c.second;
    }
    chunks.clear();
    cellSize = 0;
    chunkCells = 0;
}

void Rendering::LuminChunkCollection::Expand(const Vec3F& InMin, const Vec3F& InMax)
{
    Coord start = PosToCoord(InMin);
    Coord end = PosToCoord(InMax);
    for (int z = start.pos.z; z < end.pos.z; z++)
    for (int y = start.pos.y; y < end.pos.y; y++)
    for (int x = start.pos.x; x < end.pos.x; x++)
    {
        Coord coord(x, y, z);
        if (!chunks.contains(coord.key))
        {
            auto chunk = new LuminChunk();
            chunks[coord.key] = chunk;
            chunk->Init(chunkCells);
        }
    }
}

Vec3F Rendering::LuminChunkCollection::RefreshOldestProbe(const Vec3F& InMin, const Vec3F& InMax)
{
    Expand(InMin, InMax);
    
    LuminChunk* smallest = nullptr;
    Coord smallestCoord = Coord(0);
    float smallestTime = -1;
    Coord start = PosToCoord(InMin);
    Coord end = PosToCoord(InMax);
    for (int z = start.pos.z; z < end.pos.z; z++)
    for (int y = start.pos.y; y < end.pos.y; y++)
    for (int x = start.pos.x; x < end.pos.x; x++)
    {
        Coord coord(x, y, z);
        LuminChunk* c = chunks.at(coord.key);
        const float t = c->GetOldestTimestamp();
        if (t < smallestTime || smallestTime < 0 || smallest == nullptr)
        {
            smallestTime = t;
            smallest = c;
            smallestCoord = coord;
        }
    }
    if (!smallest)
        return Vec3F();
    
    auto probeCoord = smallest->RefreshOldestCell();
    Vec3F probePos = Vec3F( 
        probeCoord.pos.x,
        probeCoord.pos.y,
        probeCoord.pos.z
    ) * cellSize;
    Vec3F chunkSize = cellSize * chunkCells;
    Vec3F chunkPos = Vec3F(
        smallestCoord.pos.x,
        smallestCoord.pos.y,
        smallestCoord.pos.z
    ) * chunkSize;
    return probePos + chunkPos;
}

Vector<Rendering::LuminChunkFrameData> Rendering::LuminChunkCollection::GetFrameChunks(const Frustum &InFrustum, const Vec3F& InMin, const Vec3F& InMax)
{
    Expand(InMin, InMax);
    
    Vector<Rendering::LuminChunkFrameData> result;
    
    Coord start = PosToCoord(InMin);
    Coord end = PosToCoord(InMax);
    for (int z = start.pos.z; z < end.pos.z; z++)
    for (int y = start.pos.y; y < end.pos.y; y++)
    for (int x = start.pos.x; x < end.pos.x; x++)
    {
        Coord coord(x, y, z);
        Vec3F chunkSize = cellSize * chunkCells;
        Vec3F chunkPos = Vec3F(
            coord.pos.x,
            coord.pos.y,
            coord.pos.z
        ) * chunkSize;
        CHECK_CONTINUE(!InFrustum.CheckCube())
        
        
        
        LuminChunk* c = chunks.at(coord.key);
    }
}

Rendering::LuminChunkCollection::Coord Rendering::LuminChunkCollection::PosToCoord(const Vec3F &InPos) const
{
    Vec3F chunkSize = cellSize * chunkCells;
    return (
        std::round(InPos.x / chunkSize.x),
        std::round(InPos.y / chunkSize.y),
        std::round(InPos.z / chunkSize.z)
    );
}
