#include "Deserialize.h"

#include "Utility/JsonUtility.h"

#define DESERIALIZE_CHECK_RETURN(cond, error) \
if (DeserializePrint) { \
    CHECK_RETURN_LOG(cond, error, false); \
} else { \
    CHECK_RETURN(cond, false) \
} \

bool Utility::Deserialize(const DeserializeObj& InContent, const String& InName, bool& OutData)
{
    DESERIALIZE_CHECK_RETURN(!InContent.HasMember(InName.c_str()), "No member: " + InName);
    DESERIALIZE_CHECK_RETURN(!InContent[InName.c_str()].IsBool(), "Incorrect type, expected bool");
    OutData = InContent[InName.c_str()].GetBool();
    return true;
}

bool Utility::Deserialize(const DeserializeObj& InContent, const String& InName, float& OutData)
{
    DESERIALIZE_CHECK_RETURN(!InContent.HasMember(InName.c_str()), "No member: " + InName);
    DESERIALIZE_CHECK_RETURN(!InContent[InName.c_str()].IsFloat(), "Incorrect type, expected float");
    OutData = InContent[InName.c_str()].GetFloat();
    return true;
}

bool Utility::Deserialize(const DeserializeObj& InContent, const String& InName, int32& OutData)
{
    DESERIALIZE_CHECK_RETURN(!InContent.HasMember(InName.c_str()), "No member: " + InName);
    DESERIALIZE_CHECK_RETURN(!InContent[InName.c_str()].IsInt(), "Incorrect type, expected int");
    OutData = InContent[InName.c_str()].GetInt();
    return true;
}

bool Utility::Deserialize(const DeserializeObj& InContent, const String& InName, Vec3F& OutData)
{
    DESERIALIZE_CHECK_RETURN(!InContent.HasMember(InName.c_str()), "No member: " + InName);
    DESERIALIZE_CHECK_RETURN(!InContent[InName.c_str()].IsArray(), "Incorrect type, expected array");
    const auto arr = InContent[InName.c_str()].GetArray();
    DESERIALIZE_CHECK_RETURN(arr.Size() != 3, "Invalid array size: " + std::to_string(arr.Size()));
    for (int32 i = 0; i < 3; i++)
    {
        DESERIALIZE_CHECK_RETURN(!arr[i].IsFloat(), "Array incorrect type, expected float");
        OutData.data[i] = arr[i].GetFloat();
    }
    return true;
}

bool Utility::Deserialize(const DeserializeObj& InContent, const String& InName, QuatF& OutData)
{
    DESERIALIZE_CHECK_RETURN(!InContent.HasMember(InName.c_str()), "No member: " + InName);
    DESERIALIZE_CHECK_RETURN(!InContent[InName.c_str()].IsArray(), "Incorrect type, expected array");
    const auto arr = InContent[InName.c_str()].GetArray();
    DESERIALIZE_CHECK_RETURN(arr.Size() != 3, "Invalid array size: " + std::to_string(arr.Size()));
    for (int32 i = 0; i < 4; i++)
    {
        DESERIALIZE_CHECK_RETURN(!arr[i].IsFloat(), "Array incorrect type, expected float");
        OutData.data[i] = arr[i].GetFloat();
    }
    return true;
}

bool Utility::Deserialize(const DeserializeObj& InContent, const String& InName, Mat4F& OutData)
{
    DESERIALIZE_CHECK_RETURN(!InContent.HasMember(InName.c_str()), "No member: " + InName);
    DESERIALIZE_CHECK_RETURN(!InContent[InName.c_str()].IsArray(), "Incorrect type, expected array");
    const auto arr = InContent[InName.c_str()].GetArray();
    DESERIALIZE_CHECK_RETURN(arr.Size() != 3, "Invalid array size: " + std::to_string(arr.Size()));
    for (int32 i = 0; i < 16; i++)
    {
        DESERIALIZE_CHECK_RETURN(!arr[i].IsFloat(), "Array incorrect type, expected float");
        OutData.data[i] = arr[i].GetFloat();
    }
    return true;
}

bool Utility::Deserialize(const DeserializeObj& InContent, const String& InName, String& OutData)
{
    DESERIALIZE_CHECK_RETURN(!InContent.HasMember(InName.c_str()), "No member: " + InName);
    DESERIALIZE_CHECK_RETURN(!InContent[InName.c_str()].IsString(), "Incorrect type, expected string");
    OutData = InContent[InName.c_str()].GetString();
    return true;
}

#undef DESERIALIZE_CHECK_RETURN