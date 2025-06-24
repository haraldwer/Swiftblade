#pragma once
#include <filesystem>
#include <fstream>

#ifndef PACK_PATH
#define PACK_PATH "C:\\Dev\\content"
#endif

#ifndef PACK_CHUNK_SIZE
#define PACK_CHUNK_SIZE (1 << 16)
#endif

#ifndef PACK_KEY
#define PACK_KEY (12341234)
#endif

namespace Utility
{
    struct Asset : PropertyOwner<Asset>
    {
        PROPERTY_D(uint32, Hash, 0);
        PROPERTY_D(uint64, Start, 0);
        PROPERTY_D(uint64, End, 0);
        PROPERTY_D(uint64, StartChunk, 0);
        PROPERTY_D(uint64, EndChunk, 0);
    };

    struct AssetRegistry : PropertyOwner<AssetRegistry>
    {
        PROPERTY(Vector<Asset>, Assets);
    };
    
    class AssetPackGenerator
    {
    public:
        bool Package();

    private:
        bool ProcessDir(const String& InPath);
        bool ProcessFile(const std::filesystem::directory_entry& InEntry);
        
        bool BeginChunk();
        bool EndChunk();
        
        std::ofstream currentChunk = {};
        uint64 chunkCount = 0;
        uint64 writeOffset = 0;
        
        AssetRegistry assets = {};
    };

    class AssetPackReader
    {
    public:
        AssetPackReader();
        String Read(const String& InFile) const;

    private:
        static String ReadChunk(uint64 InChunk, uint64 InStart, uint64 InEnd);
        Map<uint32, Asset> assets = {};
        
    };
}

