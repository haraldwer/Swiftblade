#pragma once

//#define DESERIALIZE_PRINT

#ifdef DESERIALIZE_PRINT
#define DESERIALIZE_CHECK_RETURN(cond, error) CHECK_RETURN_LOG(cond, error, false);
#else
#define DESERIALIZE_CHECK_RETURN(cond, error) CHECK_RETURN(cond, false)
#endif

namespace Utility
{
    
    inline bool ReadValue(const GenericVal& InVal, bool& OutData)
    {
        DESERIALIZE_CHECK_RETURN(!InVal.IsBool(), "Incorrect type, expected bool");
        OutData = InVal.GetBool();
        return true; 
    }
    
    inline bool ReadValue(const GenericVal& InVal, float& OutData)
    {
        if (InVal.IsFloat())
        {
            OutData = InVal.GetFloat();
            return true;
        }
        
        // Try int
        DESERIALIZE_CHECK_RETURN(!InVal.IsInt(), "Incorrect type, expected number")
        OutData = static_cast<float>(InVal.GetInt());
        return true;
    }
    
    inline bool ReadValue(const GenericVal& InVal, int32& OutData)
    {
        DESERIALIZE_CHECK_RETURN(!InVal.IsInt(), "Incorrect type, expected int");
        OutData = InVal.GetInt();
        return true; 
    }
    
    inline bool ReadValue(const GenericVal& InVal, uint8& OutData)
    {
        DESERIALIZE_CHECK_RETURN(!InVal.IsUint(), "Incorrect type, expected uint");
        OutData = static_cast<uint8>(InVal.GetUint());
        return true;
    }

    inline bool ReadValue(const GenericVal& InVal, uint32& OutData)
    {
        DESERIALIZE_CHECK_RETURN(!InVal.IsUint(), "Incorrect type, expected uint");
        OutData = InVal.GetUint();
        return true;
    }

    inline bool ReadValue(const GenericVal& InVal, uint64& OutData)
    {
        DESERIALIZE_CHECK_RETURN(!InVal.IsUint(), "Incorrect type, expected uint");
        OutData = InVal.GetUint();
        return true;
    }

    template <typename T>
    bool ReadValue(const GenericVal& InVal, Math::Vector2<T>& OutData)
    {
        DESERIALIZE_CHECK_RETURN(!InVal.IsArray(), "Incorrect type, expected Arr");
        const auto arr = InVal.GetArray();
        DESERIALIZE_CHECK_RETURN(arr.Size() != 2, "Invalid array size: " + std::to_string(arr.Size()));
        for (int32 i = 0; i < 2; i++)
            DESERIALIZE_CHECK_RETURN(!ReadValue(arr[i], OutData.data[i]), "Array incorrect type, expected float");
        return true;
    }

    template <typename T>
    bool ReadValue(const GenericVal& InVal, Math::Vector3<T>& OutData)
    {
        DESERIALIZE_CHECK_RETURN(!InVal.IsArray(), "Incorrect type, expected Arr");
        const auto arr = InVal.GetArray();
        DESERIALIZE_CHECK_RETURN(arr.Size() != 3, "Invalid array size: " + std::to_string(arr.Size()));
        for (int32 i = 0; i < 3; i++)
            DESERIALIZE_CHECK_RETURN(!ReadValue(arr[i], OutData.data[i]), "Array incorrect type, expected float");
        return true;
    }

    template <typename T>
    bool ReadValue(const GenericVal& InVal, Math::Vector4<T>& OutData)
    {
        DESERIALIZE_CHECK_RETURN(!InVal.IsArray(), "Incorrect type, expected Arr");
        const auto arr = InVal.GetArray();
        DESERIALIZE_CHECK_RETURN(arr.Size() != 4, "Invalid array size: " + std::to_string(arr.Size()));
        for (int32 i = 0; i < 4; i++)
            DESERIALIZE_CHECK_RETURN(!ReadValue(arr[i], OutData.data[i]), "Array incorrect type, expected float");
        return true;
    }

    template <typename T>
    bool ReadValue(const GenericVal& InVal, Math::Quaternion<T>& OutData)
    {
        Math::Vector3<T> euler = OutData.Euler(); 
        if (ReadValue(InVal, euler))
        {
            OutData = QuatF::FromEuler(euler * Math::DegreesToRadians(1.0f));        
            return true; 
        }
        return false; 
    }

    template <typename T>
    bool ReadValue(const GenericVal& InVal, Math::Matrix4x4<T>& OutData)
    {
        DESERIALIZE_CHECK_RETURN(!InVal.IsArray(), "Incorrect type, expected Arr");
        const auto arr = InVal.GetArray();
        DESERIALIZE_CHECK_RETURN(arr.Size() != 16, "Invalid array size: " + std::to_string(arr.Size()));
        for (int32 i = 0; i < 16; i++)
            DESERIALIZE_CHECK_RETURN(!ReadValue(arr[i], OutData.data[i]), "Array incorrect type, expected float");
        return true;
    }

    inline bool ReadValue(const GenericVal& InVal, String& OutData)
    {
        DESERIALIZE_CHECK_RETURN(!InVal.IsString(), "Incorrect type, expected string");
        OutData = InVal.GetString();
        return true; 
    }

    template <class T>
    bool ReadValue(const GenericVal& InVal, T& OutData)
    {
        return OutData.Deserialize(InVal);
    }
    
    // What type is the enums? 

    template <class T>
    bool ReadValue(const GenericVal& InVal, Vector<T>& OutData)
    {
        DESERIALIZE_CHECK_RETURN(!InVal.IsArray(), "Incorrect type, expected arr");
        for (auto& entry : InVal.GetArray())
            ReadValue(entry, OutData.emplace_back());
        return true; 
    }

    template <class T, int Size>
    bool ReadValue(const GenericVal& InVal, Array<T, Size>& OutData)
    {
        DESERIALIZE_CHECK_RETURN(!InVal.IsArray(), "Incorrect type, expected arr");
        int index = 0;
        for (auto& entry : InVal.GetArray())
        {
            if (index >= Size)
                break;
            T& data = OutData[index];  
            ReadValue(entry, data);
            index++;
        }
        return true; 
    }

    template <class T>
    bool ReadValue(const GenericVal& InVal, Set<T>& OutData)
    {
        DESERIALIZE_CHECK_RETURN(!InVal.IsArray(), "Incorrect type, expected arr");
        for (auto& entry : InVal.GetArray())
        {
            T data; 
            if (ReadValue(entry, data))
                OutData.insert(data); 
        }
        return true; 
    }

    template <class K, class V>
    bool ReadValue(const GenericVal& InVal, Map<K, V>& OutData)
    {
        DESERIALIZE_CHECK_RETURN(!InVal.IsArray(), "Incorrect type, expected arr");
        for (auto& entry : InVal.GetArray())
        {
            DESERIALIZE_CHECK_RETURN(!entry.IsObject(), "Incorrect type, expected object");
            DESERIALIZE_CHECK_RETURN(!entry.HasMember("Key"), "No key");
            DESERIALIZE_CHECK_RETURN(!entry.HasMember("Val"), "No value");

            K key;
            V val;
            if (ReadValue(entry["Key"], key))
            {
                // Create entry even if value fails to read
                ReadValue(entry["Val"], val);
                OutData[key] = val;
            }
        }
        return true; 
    }

    template <class K, class V>
    bool ReadValue(const GenericVal& InVal, OrderedMap<K, V>& OutData)
    {
        DESERIALIZE_CHECK_RETURN(!InVal.IsArray(), "Incorrect type, expected arr");
        for (auto& entry : InVal.GetArray())
        {
            DESERIALIZE_CHECK_RETURN(!entry.IsObject(), "Incorrect type, expected object");
            DESERIALIZE_CHECK_RETURN(!entry.HasMember("Key"), "No key");
            DESERIALIZE_CHECK_RETURN(!entry.HasMember("Val"), "No value");

            K key;
            V val;
            if (ReadValue(entry["Key"], key))
            {
                // Create entry even if value fails to read
                ReadValue(entry["Val"], val);
                OutData[key] = val;
            }
        }
        return true; 
    }
    
    template <class T>
    bool Deserialize(const DeserializeObj& InContent, const String& InName, T& OutData)
    {
        DESERIALIZE_CHECK_RETURN(!InContent.HasMember(InName.c_str()), "No member: " + InName);
        return ReadValue(InContent[InName.c_str()], OutData); 
    }
}
