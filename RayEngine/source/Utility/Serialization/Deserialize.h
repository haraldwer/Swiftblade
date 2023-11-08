#pragma once

namespace Utility
{
    static constexpr bool DeserializePrint = false; 
    
    template <class T>
    bool Deserialize(const DeserializeObj& InObj, const String& InName, T& OutData)
    {
        return OutData.Deserialize(InObj, InName); 
    }

    bool Deserialize(const DeserializeObj& InContent, const String& InName, bool& OutData);
    bool Deserialize(const DeserializeObj& InContent, const String& InName, float& OutData);
    bool Deserialize(const DeserializeObj& InContent, const String& InName, int32& OutData);
    bool Deserialize(const DeserializeObj& InContent, const String& InName, Vec3F& OutData);
    bool Deserialize(const DeserializeObj& InContent, const String& InName, QuatF& OutData);
    bool Deserialize(const DeserializeObj& InContent, const String& InName, Mat4F& OutData);
    bool Deserialize(const DeserializeObj& InContent, const String& InName, String& OutData);
}
