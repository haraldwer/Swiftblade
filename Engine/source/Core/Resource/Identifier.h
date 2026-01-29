#pragma once
#include "Utility/Math/Hash.h"

namespace Resource
{
    class ID
    {
    public:
        ID() = default;
        ID(const String& InStr, const bool InUnique = false)
        {
            hash = InUnique ? 0 : Utility::Hash(InStr);
            str = InStr;
        }

        String Str() const { return str; }
        bool IsValid() const { return !str.empty(); }
        bool Unique() const { return hash == 0; }
        uint32 Hash() const { return hash; }

        void Serialize(SerializeObj& InObj);
        bool Deserialize(const DeserializeObj& InObj);

        bool operator==(const ID& InOther) const
        {
            return InOther.hash == hash;
        }
        
    private:
        String str = {};
        uint32 hash = 0;
    };
}
