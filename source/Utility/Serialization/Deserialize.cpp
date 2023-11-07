#include "Deserialize.h"

#include "Utility/JsonUtility.h"

bool Utility::Deserialize(const DeserializeObj& InContent, const String& InName, bool& OutData)
{
    CHECK_RETURN_LOG(!InContent.HasMember(InName.c_str()), "No member: " + InName, false);
    CHECK_RETURN_LOG(!InContent[InName.c_str()].IsBool(), "Incorrect type, expected bool", false);
    OutData = InContent[InName.c_str()].GetBool();
    return true;
}

bool Utility::Deserialize(const DeserializeObj& InContent, const String& InName, float& OutData)
{
    CHECK_RETURN_LOG(!InContent.HasMember(InName.c_str()), "No member: " + InName, false);
    CHECK_RETURN_LOG(!InContent[InName.c_str()].IsFloat(), "Incorrect type, expected float", false);
    OutData = InContent[InName.c_str()].GetFloat();
    return true;
}

bool Utility::Deserialize(const DeserializeObj& InContent, const String& InName, int32& OutData)
{
    CHECK_RETURN_LOG(!InContent.HasMember(InName.c_str()), "No member: " + InName, false);
    CHECK_RETURN_LOG(!InContent[InName.c_str()].IsInt(), "Incorrect type, expected int", false);
    OutData = InContent[InName.c_str()].GetInt();
    return true;
}

bool Utility::Deserialize(const DeserializeObj& InContent, const String& InName, Vec3F& OutData)
{
    CHECK_RETURN_LOG(!InContent.HasMember(InName.c_str()), "No member: " + InName, false);
    CHECK_RETURN_LOG(!InContent[InName.c_str()].IsArray(), "Incorrect type, expected array", false);
    const auto arr = InContent[InName.c_str()].GetArray();
    CHECK_RETURN_LOG(arr.Size() != 3, "Invalid array size: " + std::to_string(arr.Size()), false);
    for (int32 i = 0; i < 3; i++)
    {
        CHECK_RETURN_LOG(!arr[i].IsFloat(), "Array incorrect type, expected float", false);
        OutData.data[i] = arr[i].GetFloat();
    }
    return true;
}

bool Utility::Deserialize(const DeserializeObj& InContent, const String& InName, QuatF& OutData)
{
    CHECK_RETURN_LOG(!InContent.HasMember(InName.c_str()), "No member: " + InName, false);
    CHECK_RETURN_LOG(!InContent[InName.c_str()].IsArray(), "Incorrect type, expected array", false);
    const auto arr = InContent[InName.c_str()].GetArray();
    CHECK_RETURN_LOG(arr.Size() != 3, "Invalid array size: " + std::to_string(arr.Size()), false);
    for (int32 i = 0; i < 4; i++)
    {
        CHECK_RETURN_LOG(!arr[i].IsFloat(), "Array incorrect type, expected float", false);
        OutData.data[i] = arr[i].GetFloat();
    }
    return true;
}

bool Utility::Deserialize(const DeserializeObj& InContent, const String& InName, Mat4F& OutData)
{
    CHECK_RETURN_LOG(!InContent.HasMember(InName.c_str()), "No member: " + InName, false);
    CHECK_RETURN_LOG(!InContent[InName.c_str()].IsArray(), "Incorrect type, expected array", false);
    const auto arr = InContent[InName.c_str()].GetArray();
    CHECK_RETURN_LOG(arr.Size() != 3, "Invalid array size: " + std::to_string(arr.Size()), false);
    for (int32 i = 0; i < 16; i++)
    {
        CHECK_RETURN_LOG(!arr[i].IsFloat(), "Array incorrect type, expected float", false);
        OutData.data[i] = arr[i].GetFloat();
    }
    return true;
}

bool Utility::Deserialize(const DeserializeObj& InContent, const String& InName, String& OutData)
{
    CHECK_RETURN_LOG(!InContent.HasMember(InName.c_str()), "No member: " + InName, false);
    CHECK_RETURN_LOG(!InContent[InName.c_str()].IsString(), "Incorrect type, expected string", false);
    OutData = InContent[InName.c_str()].GetString();
    return true;
}