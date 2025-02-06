#include "AssetPackGenerator.h"

#include <filesystem>

Utility::AssetPackGenerator::AssetPackGenerator(const String& InTarget, uint64 InChunkSize) : ChunkSize(InChunkSize), OutPath(InTarget)
{
}

bool Utility::AssetPackGenerator::Package()
{
    LOG("Packaging assets");
    CHECK_RETURN_LOG(CurrentChunk.is_open(), "Chunk already open", false);
    ChunkCount = 0;
    WriteOffset = 0;
    bool result = ProcessDir(std::filesystem::current_path().string());
    LOG(result ? "Package Success" : "Package Failure");
    CreateAssetRegistry();
    return result;
}

bool Utility::AssetPackGenerator::BeginChunk()
{
    LOG("Opening chunk: " + OutPath);
    String path = OutPath + "_" + std::to_string(ChunkCount);
    CurrentChunk.open(path);
    return CurrentChunk.is_open();
}

bool Utility::AssetPackGenerator::EndChunk()
{
    CurrentChunk.close();
    ChunkCount++;
    WriteOffset = 0;
    LOG("Closing chunk");
    return !CurrentChunk.is_open();
}

bool Utility::AssetPackGenerator::ProcessDir(const String& InPath)
{
    LOG("Iterating directory: " + InPath);
    bool result = true;
    for (auto& entry : std::filesystem::directory_iterator(InPath))
    {
        if (entry.is_directory())
        {
            if (!ProcessDir(entry.path().string()))
                result = false;
        }
        else if (!ProcessFile(entry))
            result = false;
    }
    return result;
}

bool Utility::AssetPackGenerator::ProcessFile(const std::filesystem::directory_entry& InEntry)
{
    if (InEntry.path() == OutPath)
        return true;

    if (!CurrentChunk.is_open())
        if (!BeginChunk())
            return false;
    
    LOG("Opening file: " + InEntry.path().string());
    auto stream = std::ifstream(InEntry.path().string());
    stream.exceptions(std::ios_base::badbit);
    CHECK_RETURN_LOG(!stream, "ERROR: Stream exception", false);

    auto& asset = Assets.emplace_back();
    asset.Path = InEntry.path().string();
    asset.Start = WriteOffset;
    asset.StartChunk = ChunkCount;

    auto write = [&](const std::string& InBuff, uint64 InCount)
    {
        const uint64 totalOffset = WriteOffset + InCount;
        const uint64 firstWrite = Math::Min(totalOffset, ChunkSize) - WriteOffset;
        const uint64 nextWrite = InCount - firstWrite;
         
        LOG("Writing " + std::to_string(firstWrite) + " bytes");
        CurrentChunk << InBuff.substr(0, firstWrite);
        WriteOffset += firstWrite;

        // Begin next chunk
        if (WriteOffset >= ChunkSize)
        {
            if (!EndChunk())
                return false;
            if (!BeginChunk())
                return false;
        }

        if (nextWrite > 0)
        {
            CurrentChunk << InBuff.substr(firstWrite, nextWrite);
            WriteOffset += nextWrite;
            LOG("Writing additional " + std::to_string(firstWrite) + " bytes");
        }

        return true; 
    };
    
    // Read chunks
    auto out = std::string();
    auto buf = std::string(ChunkSize, '\0');
    while (stream.read(buf.data(), ChunkSize))
        write(buf, stream.gcount());
    write(buf, stream.gcount());
    LOG("Finished reading file");

    asset.End = WriteOffset;
    asset.EndChunk = ChunkCount;
    
    return true;
}

void Utility::AssetPackGenerator::CreateAssetRegistry()
{
    for (auto& a : Assets)
    {
        LOG("Asset: " + a.Path);
        LOG("Start: " + std::to_string(a.Start) + " Sc: " + std::to_string(a.StartChunk) + " End: " + std::to_string(a.End) + " Ec:" + std::to_string(a.EndChunk));
    }
}
