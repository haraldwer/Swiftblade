#include "LuminChunk.h"

#include <random>

#include "Scene/Culling/Frustum.h"

void Rendering::LuminChunk::Init(int InCells)
{
    cells = InCells;
    
    // 3D textures cannot be interpolated
    if (targets03.TryBeginSetup(InCells, TextureType::TEXTURE_3D))
    {
        targets03.CreateBuffer("TexSH0");
        targets03.CreateBuffer("TexSH1");
        targets03.CreateBuffer("TexSH2");
        targets03.CreateBuffer("TexSH3");
        targets03.CreateBuffer("TexSH4");
        targets03.EndSetup();
    }
    
    if (targets48.TryBeginSetup(InCells, TextureType::TEXTURE_3D))
    {
        targets48.CreateBuffer("TexSH4");
        targets48.CreateBuffer("TexSH5");
        targets48.CreateBuffer("TexSH6");
        targets48.CreateBuffer("TexSH7");
        targets48.CreateBuffer("TexSH8");
        targets48.EndSetup();
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

void Rendering::LuminChunk::Deinit()
{
    targets03.Unload();
    targets48.Unload();
    timestamps.clear();
    timeIndices.clear();
    cells = 0;
}

float Rendering::LuminChunk::GetOldestTimestamp() const
{
    return timestamps.at(timeIndices.at(0));
}

Rendering::LuminCoord Rendering::LuminChunk::RefreshOldestCell()
{
    // Convert coord to index
    int timestampIndex = timeIndices.at(0);
    
    // Move timestamp to back
    timeIndices.erase(timeIndices.begin());
    timeIndices.push_back(timestampIndex);
    
    timestamps[timestampIndex] = Utility::Time::Get().Unscaled();
    return IndexToCoord(timestampIndex);
}

Rendering::LuminCoord Rendering::LuminChunk::IndexToCoord(const int InIndex) const
{
    LuminCoord c;
    c.pos.x = InIndex % cells;
    c.pos.y = InIndex / cells % cells;
    c.pos.z = InIndex / (cells * cells);
    return c;
}

int Rendering::LuminChunk::CoordToIndex(const LuminCoord& InCoord) const
{
    return InCoord.pos.x
         + InCoord.pos.y * cells
         + InCoord.pos.z * cells * cells;
}

void Rendering::LuminChunkCollection::Init(const int InChunkAxisCells, const Vec3F& InCellSize)
{
    CHECK_ASSERT(!chunks.empty(), "Init before unload");
    cellSize = InCellSize;
    chunkCells = InChunkAxisCells;
}

void Rendering::LuminChunkCollection::Deinit()
{
    for (auto& c : chunks)
    {
        c.second->Deinit();
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
    for (int z = start.pos.z; z <= end.pos.z; z++)
    for (int y = start.pos.y; y <= end.pos.y; y++)
    for (int x = start.pos.x; x <= end.pos.x; x++)
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

void Rendering::LuminChunkCollection::RefreshOldestProbe(const Vec3F &InMin, const Vec3F &InMax, LuminChunkFrameData& OutChunkData, Vec3F& OutCell, LuminCoord& OutCoord) const
{
    LuminChunk* smallest = nullptr;
    Coord smallestCoord = Coord(0);
    float smallestTime = -1;
    Coord start = PosToCoord(InMin);
    Coord end = PosToCoord(InMax);
    for (int16 z = start.pos.z; z <= end.pos.z; z++)
    for (int16 y = start.pos.y; y <= end.pos.y; y++)
    for (int16 x = start.pos.x; x <= end.pos.x; x++)
    { 
        const Coord coord(x, y, z);
        LuminChunk* c = chunks.at(coord.key);
        const float t = c->GetOldestTimestamp();
        if (t < smallestTime || smallestTime < 0 || smallest == nullptr)
        {
            smallestTime = t;
            smallest = c;
            smallestCoord = coord;
        }
        // TODO: Prioritize chunks based on distance
    }
    CHECK_RETURN(!smallest);
    
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
    
    OutCoord = probeCoord;
    OutCell = probePos + chunkPos;
    OutChunkData = {
        &smallest->GetTargets03(),
        &smallest->GetTargets48(),
        chunkPos
    };
}

Vector<Rendering::LuminChunkFrameData> Rendering::LuminChunkCollection::GetFrameChunks(const Frustum &InFrustum, const Vec3F& InMin, const Vec3F& InMax) const
{
    Vector<LuminChunkFrameData> result;
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
        CHECK_CONTINUE(!InFrustum.CheckBox(chunkPos, chunkSize))
        LuminChunk* c = chunks.at(coord.key);
        result.push_back({
            &c->GetTargets03(),
            &c->GetTargets48(),
            chunkPos
        });
    }
    return result;
}

Rendering::LuminChunkCollection::Coord Rendering::LuminChunkCollection::PosToCoord(const Vec3F &InPos) const
{
    const Vec3F chunkSize = cellSize * chunkCells;
    return Coord(
        static_cast<int16>(std::round(InPos.x / chunkSize.x)),
        static_cast<int16>(std::round(InPos.y / chunkSize.y)),
        static_cast<int16>(std::round(InPos.z / chunkSize.z))
    );
}
