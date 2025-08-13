#include "CubeVolumeData.h"

void ECS::CubeVolumeData::Serialize(SerializeObj& InOutObj) const
{
    Map<uint8, Vector<uint32>> entries;
    for (const auto& entry : data)
    {
        CHECK_CONTINUE(!entry.second)
        entries[entry.second].push_back(entry.first); 
    }

    String result;
    for (const auto& value : entries)
    {
        result += std::to_string(value.first) + ":";
        for (const auto& entry : value.second)
            result += std::to_string(entry) + ",";
        result += ";";
    }

    Utility::Serialize(InOutObj, "Data", result); 
}

bool ECS::CubeVolumeData::Deserialize(const DeserializeObj& InObj)
{
    // Unpack
    String stringData;
    Utility::Deserialize(InObj, "Data", stringData);

    size_t index = 0; 
    while (true)
    {
        // Find ;
        const size_t find = stringData.find(';', index);
        if (find == std::string::npos)
            break; // Has reached end!

        // Split on ; 
        const String findStr = stringData.substr(index, find - index);

        // Find :
        const size_t split = findStr.find(':');
        if (split == std::string::npos)
            break; // Has reached end!

        // Split on :
        const String valueStr = findStr.substr(0, split);
        const uint8 value = static_cast<uint8>(std::stoi(valueStr));
        const String entriesStr = findStr.substr(split + 1);

        // Find all values
        size_t valueIndex = 0; 
        while (true)
        {
            // Find ,
            const size_t entryFind = entriesStr.find(',', valueIndex);
            if (entryFind == std::string::npos)
                break; // Has reached end of entries

            // Split on ,
            const String entryStr = entriesStr.substr(valueIndex, entryFind - valueIndex);
            const uint32 entry = std::stoi(entryStr);

            // Add to data!
            data[entry] = value;
            valueIndex = entryFind + 1; 
        }
        
        index = find + 1; 
    }

    return true; 
}
