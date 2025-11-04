#include "AssetPacker.h"

#include <filesystem>

#include "Utility/File/File.h"
#include "Utility/Encrypt.h"

bool Utility::AssetPackGenerator::Package()
{
    LOG("Packaging assets");
    CHECK_RETURN_LOG(currentChunk.is_open(), "Chunk already open", false);
    chunkCount = 0;
    writeOffset = 0;
    bool result = ProcessDir(std::filesystem::current_path().string());
    if (currentChunk.is_open())
        if (!EndChunk())
            result = false;
    LOG(result ? "Package Success" : "Package Failure");

    rapidjson::StringBuffer s;
    rapidjson::Writer writer(s);
    assets.Serialize(writer);
    const String hash = std::to_string(Hash(String("reg")));
    const String path = String(PACK_PATH)  + "_" + hash;
    String content = FormatJson(s.GetString());
    Encrypt(content, 0, PACK_KEY);
    if (!WriteFile(path, content))
        result = false;

    return result;
}

bool Utility::AssetPackGenerator::BeginChunk()
{
    const String path = String(PACK_PATH) + "_" + std::to_string(Hash(chunkCount));
    LOG("Opening chunk: " + path);
    currentChunk.open(path, std::ios::out);
    return currentChunk.is_open();
}

bool Utility::AssetPackGenerator::EndChunk()
{
    LOG("Closing chunk, size: " + std::to_string(writeOffset));
    currentChunk.close();
    chunkCount++;
    writeOffset = 0;
    return !currentChunk.is_open();
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
    if (InEntry.path() == String(PACK_PATH))
        return true;

    if (!currentChunk.is_open())
        if (!BeginChunk())
            return false;
    
    String path = InEntry.path().string();
    path = path.substr(std::filesystem::current_path().string().length() + 1);
    
    LOG("Opening file: " + path);
    auto stream = std::ifstream(path, std::ios::in);
    stream.exceptions(std::ios_base::badbit);
    CHECK_RETURN_LOG(!stream, "ERROR: Stream exception", false);

    auto& asset = assets.Assets.Get().emplace_back();
    asset.Hash = Hash(path);
    asset.Start = writeOffset;
    asset.StartChunk = chunkCount;

    auto write = [&](const String& InBuff, const uint64 InCount)
    {
        const uint64 totalOffset = writeOffset + InCount;
        const uint64 firstWrite = Math::Min(totalOffset, static_cast<uint64>(PACK_CHUNK_SIZE)) - writeOffset;
        const uint64 nextWrite = InCount - firstWrite;

        LOG("Writing " + std::to_string(firstWrite) + " bytes");
        String firstStr = InBuff.substr(0, firstWrite);
        Encrypt(firstStr, writeOffset, PACK_KEY);
        currentChunk << firstStr;
        writeOffset += firstWrite;

        // Begin next chunk
        if (writeOffset >= PACK_CHUNK_SIZE)
        {
            CHECK_RETURN_LOG(!EndChunk(), "Failed to end chunk", false);
            CHECK_RETURN_LOG(!BeginChunk(), "Failed to begin chunk", false);
        }

        if (nextWrite > 0)
        {
            String secondStr = InBuff.substr(firstWrite, nextWrite);
            Encrypt(secondStr, writeOffset, PACK_KEY);
            currentChunk << secondStr;
            writeOffset += nextWrite;
            LOG("Writing additional " + std::to_string(firstWrite) + " bytes");
        }

        return true;
    };

    // Read chunks
    String buf = String(PACK_CHUNK_SIZE, '\0');
    while (stream.read(buf.data(), PACK_CHUNK_SIZE))
        write(buf, stream.gcount());
    write(buf, stream.gcount());
    LOG("Finished reading file");

    asset.End = writeOffset;
    asset.EndChunk = chunkCount;

    return true;
}

Utility::AssetPackReader::AssetPackReader()
{
    AssetRegistry registry;

    const String hash = std::to_string(Hash(String("reg")));
    const String path = String(PACK_PATH)  + "_" + hash;
    String fileContent = ReadFile(path);
    CHECK_RETURN_LOG(fileContent.empty(), "Pack file empty");
    Decrypt(fileContent, 0, PACK_KEY);
    rapidjson::Document doc;
    doc.Parse(fileContent.c_str());
    CHECK_RETURN_LOG(!doc.IsObject(), "Invalid format");
    const rapidjson::Document& constDoc = doc;
    registry.Deserialize(constDoc.GetObj());
    
    CHECK_RETURN_LOG(registry.Assets.Get().empty(), "Failed to load assets");
    for (auto& a : registry.Assets.Get())
        assets[a.Hash.Get()] = a;
}

String Utility::AssetPackReader::Read(const String& InFile) const
{
    LOG("Reading file: " + InFile)
    const auto find = assets.find(Hash(InFile));
    CHECK_RETURN_LOG(find == assets.end(), "Unknown file: " + InFile, "");

    const Asset& asset = find->second;
    String data;
    for (uint64 chunk = asset.StartChunk; chunk <= asset.EndChunk; chunk++)
    {
        const uint64 start = chunk == asset.StartChunk ?
            asset.Start : static_cast<uint64>(0);
        const uint64 end = chunk == asset.EndChunk ?
            asset.End : static_cast<uint64>(PACK_CHUNK_SIZE);
        data += ReadChunk(chunk, start, end);
    }
    return data;
}

String Utility::AssetPackReader::ReadChunk(uint64 InChunk, uint64 InStart, uint64 InEnd)
{
    String path = String(PACK_PATH) + "_" + std::to_string(Hash(InChunk));
    std::ifstream stream;
    stream.open(path, std::ios::in);
    if (!stream.is_open())
        return "";
    String content;
    String buf = String(PACK_CHUNK_SIZE, '\0');
    while (stream.read(buf.data(), PACK_CHUNK_SIZE) && content.length() < InEnd)
        content.append(buf, 0, stream.gcount());
    content.append(buf, 0, stream.gcount());
    String chunk = buf.substr(InStart, InEnd - InStart);
    Decrypt(chunk, InStart, PACK_KEY);
    return chunk;
}

void Utility::AssetPacker::DrawDebugPanel()
{
    ImGui::Text("This tool will package game assets");
    ImGui::Text("Package can be found at:");
    ImGui::Text(PACK_PATH);
    
    if (ImGui::Button("Package", ImVec2(-1, 0)))
    {
        AssetPackGenerator generator;
        generator.Package();
        packaged = true;
    }
    
    if (packaged)
        ImGui::Text("Completed!");
}
