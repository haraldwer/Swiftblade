#include "Deserialize.h"

#include "Utility/JsonUtility.h"

bool Utility::ReadValue(const GenericVal& InVal, bool& OutData)
{
    DESERIALIZE_CHECK_RETURN(!InVal.IsBool(), "Incorrect type, expected bool");
    OutData = InVal.GetBool();
    return true; 
}

bool Utility::ReadValue(const GenericVal& InVal, float& OutData)
{
    DESERIALIZE_CHECK_RETURN(!InVal.IsFloat(), "Incorrect type, expected float");
    OutData = InVal.GetFloat();
    return true; 
}

bool Utility::ReadValue(const GenericVal& InVal, int32& OutData)
{
    DESERIALIZE_CHECK_RETURN(!InVal.IsInt(), "Incorrect type, expected int");
    OutData = InVal.GetInt();
    return true; 
}

bool Utility::ReadValue(const GenericVal& InVal, uint8& OutData)
{
    DESERIALIZE_CHECK_RETURN(!InVal.IsUint(), "Incorrect type, expected uint");
    OutData = static_cast<uint8>(InVal.GetUint());
    return true;
}

bool Utility::ReadValue(const GenericVal& InVal, uint32& OutData)
{
    DESERIALIZE_CHECK_RETURN(!InVal.IsUint(), "Incorrect type, expected uint");
    OutData = InVal.GetUint();
    return true;
}

bool Utility::ReadValue(const GenericVal& InVal, Vec3F& OutData)
{
    DESERIALIZE_CHECK_RETURN(!InVal.IsArray(), "Incorrect type, expected Arr");
    const auto arr = InVal.GetArray();
    DESERIALIZE_CHECK_RETURN(arr.Size() != 3, "Invalid array size: " + std::to_string(arr.Size()));
    for (int32 i = 0; i < 3; i++)
    {
        DESERIALIZE_CHECK_RETURN(!arr[i].IsFloat(), "Array incorrect type, expected float");
        OutData.data[i] = arr[i].GetFloat();
    }
    return true;
}

bool Utility::ReadValue(const GenericVal& InVal, Vec4F& OutData)
{
    DESERIALIZE_CHECK_RETURN(!InVal.IsArray(), "Incorrect type, expected Arr");
    const auto arr = InVal.GetArray();
    DESERIALIZE_CHECK_RETURN(arr.Size() != 4, "Invalid array size: " + std::to_string(arr.Size()));
    for (int32 i = 0; i < 4; i++)
    {
        DESERIALIZE_CHECK_RETURN(!arr[i].IsFloat(), "Array incorrect type, expected float");
        OutData.data[i] = arr[i].GetFloat();
    }
    return true;
}

bool Utility::ReadValue(const GenericVal& InVal, QuatF& OutData)
{
    DESERIALIZE_CHECK_RETURN(!InVal.IsArray(), "Incorrect type, expected Arr");
    const auto arr = InVal.GetArray();
    DESERIALIZE_CHECK_RETURN(arr.Size() != 4, "Invalid array size: " + std::to_string(arr.Size()));
    for (int32 i = 0; i < 4; i++)
    {
        DESERIALIZE_CHECK_RETURN(!arr[i].IsFloat(), "Array incorrect type, expected float");
        OutData.data[i] = arr[i].GetFloat();
    }
    return true;
}

bool Utility::ReadValue(const GenericVal& InVal, Mat4F& OutData)
{
    DESERIALIZE_CHECK_RETURN(!InVal.IsArray(), "Incorrect type, expected Arr");
    const auto arr = InVal.GetArray();
    DESERIALIZE_CHECK_RETURN(arr.Size() != 16, "Invalid array size: " + std::to_string(arr.Size()));
    for (int32 i = 0; i < 16; i++)
    {
        DESERIALIZE_CHECK_RETURN(!arr[i].IsFloat(), "Array incorrect type, expected float");
        OutData.data[i] = arr[i].GetFloat();
    }
    return true;
}

bool Utility::ReadValue(const GenericVal& InVal, String& OutData)
{
    DESERIALIZE_CHECK_RETURN(!InVal.IsString(), "Incorrect type, expected string");
    OutData = InVal.GetString();
    return true; 
}
