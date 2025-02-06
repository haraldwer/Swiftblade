#pragma once
#include <filesystem>
#include <fstream>

namespace Utility
{
    class AssetPackGenerator
    {
        struct Asset
        {
            String Path;
            uint64 Start;
            uint64 End;
            uint64 StartChunk;
            uint64 EndChunk;
        };
        
    public:
        AssetPackGenerator(const String& InTarget, uint64 InChunkSize = 1 << 16);
        bool Package();

    private:
        bool ProcessDir(const String& InPath);
        bool ProcessFile(const std::filesystem::directory_entry& InEntry);
        
        bool BeginChunk();
        bool EndChunk();
        
        const uint64 ChunkSize;
        const String OutPath;
        
        std::ofstream CurrentChunk;
        uint64 ChunkCount = 0;
        uint64 WriteOffset = 0;
        
        void CreateAssetRegistry();
        Vector<Asset> Assets;
    };

    class AssetPackReader
    {
    public:
        AssetPackReader(const String& InTarget);
        
        
    };
}

