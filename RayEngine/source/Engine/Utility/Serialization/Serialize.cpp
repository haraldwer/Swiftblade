#include "Serialize.h"

#include "Utility/JsonUtility.h"
#include "Utility/Math/AngleConversion.h"

void Utility::WriteValue(SerializeObj& InOutObj, const bool& InData)
{
    InOutObj.Bool(InData); 
}

void Utility::WriteValue(SerializeObj& InOutObj, const float& InData)
{
    InOutObj.Double(InData); 
}

void Utility::WriteValue(SerializeObj& InOutObj, const int32& InData)
{
    InOutObj.Int(InData); 
}

void Utility::WriteValue(SerializeObj& InOutObj, const uint8& InData)
{
    InOutObj.Uint(InData); 
}

void Utility::WriteValue(SerializeObj& InOutObj, const uint32& InData)
{
    InOutObj.Uint(InData); 
}

void Utility::WriteValue(SerializeObj& InOutObj, const uint64& InData)
{
    InOutObj.Uint(static_cast<unsigned>(InData)); 
}

void Utility::WriteValue(SerializeObj& InOutObj, const Vec2F& InData)
{
    InOutObj.StartArray();
    InOutObj.Double(InData.x); 
    InOutObj.Double(InData.y); 
    InOutObj.EndArray();
}

void Utility::WriteValue(SerializeObj& InOutObj, const Vec3F& InData)
{
    InOutObj.StartArray();
    InOutObj.Double(InData.x); 
    InOutObj.Double(InData.y); 
    InOutObj.Double(InData.z); 
    InOutObj.EndArray();
}

void Utility::WriteValue(SerializeObj& InOutObj, const Vec4F& InData)
{
    InOutObj.StartArray();
    InOutObj.Double(InData.x); 
    InOutObj.Double(InData.y); 
    InOutObj.Double(InData.z); 
    InOutObj.Double(InData.w); 
    InOutObj.EndArray();
}

void Utility::WriteValue(SerializeObj& InOutObj, const QuatF& InData)
{
    Vec3F euler = InData.Euler(); 
    euler *= Math::RadiansToDegrees(1.0f);
    WriteValue(InOutObj, euler); 
}

void Utility::WriteValue(SerializeObj& InOutObj, const Mat4F& InData)
{
    InOutObj.StartArray();
    for (const float f : InData.data)
        InOutObj.Double(f); 
    InOutObj.EndArray();
}

void Utility::WriteValue(SerializeObj& InOutObj, const String& InData)
{
    InOutObj.String(InData.c_str());
}
