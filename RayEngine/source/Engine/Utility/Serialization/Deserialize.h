#pragma once

#define DESERIALIZE_CHECK_RETURN(cond, error) \
if (DeserializePrint) { \
CHECK_RETURN_LOG(cond, error, false); \
} else { \
CHECK_RETURN(cond, false) \
} \

namespace Utility
{
    static constexpr bool DeserializePrint = false; 
    
    template <class T>
    bool ReadValue(const GenericVal& InVal, T& OutData)
    {
        return OutData.Deserialize(InVal); 
    }
    
    bool ReadValue(const GenericVal& InVal, bool& OutData);
    bool ReadValue(const GenericVal& InVal, float& OutData);
    bool ReadValue(const GenericVal& InVal, int32& OutData);
    bool ReadValue(const GenericVal& InVal, uint8& OutData);
    bool ReadValue(const GenericVal& InVal, uint32& OutData);
    bool ReadValue(const GenericVal& InVal, uint64& OutData);
    bool ReadValue(const GenericVal& InVal, Vec2F& OutData);
    bool ReadValue(const GenericVal& InVal, Vec3F& OutData);
    bool ReadValue(const GenericVal& InVal, Vec4F& OutData);
    bool ReadValue(const GenericVal& InVal, QuatF& OutData);
    bool ReadValue(const GenericVal& InVal, Mat4F& OutData);
    bool ReadValue(const GenericVal& InVal, String& OutData);
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
            ReadValue(entry, data);
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
            ReadValue(entry["Key"], key);
            OutData[key] = V();
            ReadValue(entry["Val"], OutData[key]);
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
