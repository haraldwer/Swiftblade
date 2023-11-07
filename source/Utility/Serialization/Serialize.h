#pragma once

namespace Utility
{
    template <class T>
    void Serialize(SerializeObj& InOutObj, const String& InName, const T& InData)
    { 
        InData.Serialize(InOutObj, InName); 
    }

    void Serialize(SerializeObj& InOutObj, const String& InName, const bool& InData);
    void Serialize(SerializeObj& InOutObj, const String& InName, const float& InData);
    void Serialize(SerializeObj& InOutObj, const String& InName, const int32& InData);
    void Serialize(SerializeObj& InOutObj, const String& InName, const Vec3F& InData);
    void Serialize(SerializeObj& InOutObj, const String& InName, const QuatF& InData);
    void Serialize(SerializeObj& InOutObj, const String& InName, const Mat4F& InData);
    void Serialize(SerializeObj& InOutObj, const String& InName, const String& InData);
}
