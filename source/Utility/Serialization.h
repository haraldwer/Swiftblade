#pragma once

namespace Utility
{
    // - Serialize - //
    
    template <class T>
    void Serialize(SerializeObj& InOutObj, const String& InName, const T& InData)
    { 
        InData.Serialize(InOutObj, InName); 
    }

    inline void Serialize(SerializeObj& InOutObj, const String& InName, const bool& InData)
    {
        InOutObj.Key(InName.c_str());
        InOutObj.Bool(InData); 
    }
    
    inline void Serialize(SerializeObj& InOutObj, const String& InName, const float& InData)
    {
        InOutObj.Key(InName.c_str());
        InOutObj.Double(InData); 
    }

    inline void Serialize(SerializeObj& InOutObj, const String& InName, const int32& InData)
    {
        InOutObj.Key(InName.c_str());
        InOutObj.Int(InData); 
    }

    inline void Serialize(SerializeObj& InOutObj, const String& InName, const Vec3F& InData)
    {
        InOutObj.Key(InName.c_str());
        InOutObj.StartArray();
        InOutObj.Double(InData.x); 
        InOutObj.Double(InData.y); 
        InOutObj.Double(InData.z); 
        InOutObj.EndArray();
    }

    inline void Serialize(SerializeObj& InOutObj, const String& InName, const QuatF& InData)
    {
        InOutObj.Key(InName.c_str());
        InOutObj.StartArray();
        InOutObj.Double(InData.x); 
        InOutObj.Double(InData.y); 
        InOutObj.Double(InData.z); 
        InOutObj.Double(InData.w); 
        InOutObj.EndArray();
    }

    inline void Serialize(SerializeObj& InOutObj, const String& InName, const Mat4F& InData)
    {
        InOutObj.Key(InName.c_str());
        InOutObj.StartArray();
        for (const float f : InData.data)
            InOutObj.Double(f); 
        InOutObj.EndArray();
    }

    inline void Serialize(SerializeObj& InOutObj, const String& InName, const String& InData)
    {
        InOutObj.Key(InName.c_str());
        InOutObj.String(InData.c_str());
    }
    

    
    // - Deserialize - // 
    
    template <class T>
    bool Deserialize(const DeserializeObj& InObj, const String& InName, T& OutData)
    {
        return OutData.Deserialize(InObj, InName); 
    }

    inline bool Deserialize(const DeserializeObj& InContent, const String& InName, bool& OutData)
    {
        CHECK_RETURN_LOG(!InContent.HasMember(InName.c_str()), "Json obj no memeber", false);
        CHECK_RETURN_LOG(!InContent[InName.c_str()].IsBool(), "Json obj incorrect type", false);
        OutData = InContent[InName.c_str()].GetBool();
        return true;
    }

    inline bool Deserialize(const DeserializeObj& InContent, const String& InName, float& OutData)
    {
        CHECK_RETURN_LOG(!InContent.HasMember(InName.c_str()), "Json obj no memeber", false);
        CHECK_RETURN_LOG(!InContent[InName.c_str()].IsFloat(), "Json obj incorrect type", false);
        OutData = InContent[InName.c_str()].GetFloat();
        return true;
    }

    inline bool Deserialize(const DeserializeObj& InContent, const String& InName, int32& OutData)
    {
        CHECK_RETURN_LOG(!InContent.HasMember(InName.c_str()), "Json obj no memeber", false);
        CHECK_RETURN_LOG(!InContent[InName.c_str()].IsInt(), "Json obj incorrect type", false);
        OutData = InContent[InName.c_str()].GetInt();
        return true;
    }
    
    inline bool Deserialize(const DeserializeObj& InContent, const String& InName, Vec3F& OutData)
    {
        CHECK_RETURN_LOG(!InContent.HasMember(InName.c_str()), "Json obj no memeber", false);
        CHECK_RETURN_LOG(!InContent[InName.c_str()].IsArray(), "Json obj incorrect type", false);
        const auto arr = InContent[InName.c_str()].GetArray();
        CHECK_RETURN_LOG(arr.Size() != 3, "Json invalid arr size", false);
        for (int32 i = 0; i < 3; i++)
        {
            CHECK_RETURN_LOG(!arr[i].IsFloat(), "Json arr entry incorrect type", false);
            OutData.data[i] = arr[i].GetFloat();
        }
        return true;
    }
    
    inline bool Deserialize(const DeserializeObj& InContent, const String& InName, QuatF& OutData)
    {
        CHECK_RETURN_LOG(!InContent.HasMember(InName.c_str()), "Json obj no memeber", false);
        CHECK_RETURN_LOG(!InContent[InName.c_str()].IsArray(), "Json obj incorrect type", false);
        const auto arr = InContent[InName.c_str()].GetArray();
        CHECK_RETURN_LOG(arr.Size() != 4, "Json invalid arr size", false);
        for (int32 i = 0; i < 4; i++)
        {
            CHECK_ASSERT(!arr[i].IsFloat(), "Json arr entry incorrect type");
            OutData.data[i] = arr[i].GetFloat();
        }
        return true;
    }
    
    inline bool Deserialize(const DeserializeObj& InContent, const String& InName, Mat4F& OutData)
    {
        CHECK_RETURN_LOG(!InContent.HasMember(InName.c_str()), "Json obj no memeber", false);
        CHECK_RETURN_LOG(!InContent[InName.c_str()].IsArray(), "Json obj incorrect type", false);
        const auto arr = InContent[InName.c_str()].GetArray();
        CHECK_RETURN_LOG(arr.Size() != 16, "Json invalid arr size", false);
        for (int32 i = 0; i < 16; i++)
        {
            CHECK_ASSERT(!arr[i].IsFloat(), "Json arr entry incorrect type");
            OutData.data[i] = arr[i].GetFloat();
        }
        return true;
    }
    
    inline bool Deserialize(const DeserializeObj& InContent, const String& InName, String& OutData)
    {
        CHECK_RETURN_LOG(!InContent.HasMember(InName.c_str()), "Json obj no memeber", false);
        CHECK_RETURN_LOG(!InContent[InName.c_str()].IsString(), "Json obj incorrect type", false);
        OutData = InContent[InName.c_str()].GetString();
        return true;
    }

    
}