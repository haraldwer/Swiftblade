#include "Serialize.h"

#include "Utility/JsonUtility.h"

void Utility::Serialize(SerializeObj& InOutObj, const String& InName, const bool& InData)
{
    InOutObj.Key(InName.c_str());
    InOutObj.Bool(InData); 
}

void Utility::Serialize(SerializeObj& InOutObj, const String& InName, const float& InData)
{
    InOutObj.Key(InName.c_str());
    InOutObj.Double(InData); 
}

void Utility::Serialize(SerializeObj& InOutObj, const String& InName, const int32& InData)
{
    InOutObj.Key(InName.c_str());
    InOutObj.Int(InData); 
}

void Utility::Serialize(SerializeObj& InOutObj, const String& InName, const Vec3F& InData)
{
    InOutObj.Key(InName.c_str());
    InOutObj.StartArray();
    InOutObj.Double(InData.x); 
    InOutObj.Double(InData.y); 
    InOutObj.Double(InData.z); 
    InOutObj.EndArray();
}

void Utility::Serialize(SerializeObj& InOutObj, const String& InName, const QuatF& InData)
{
    InOutObj.Key(InName.c_str());
    InOutObj.StartArray();
    InOutObj.Double(InData.x); 
    InOutObj.Double(InData.y); 
    InOutObj.Double(InData.z); 
    InOutObj.Double(InData.w); 
    InOutObj.EndArray();
}

void Utility::Serialize(SerializeObj& InOutObj, const String& InName, const Mat4F& InData)
{
    InOutObj.Key(InName.c_str());
    InOutObj.StartArray();
    for (const float f : InData.data)
        InOutObj.Double(f); 
    InOutObj.EndArray();
}

void Utility::Serialize(SerializeObj& InOutObj, const String& InName, const String& InData)
{
    InOutObj.Key(InName.c_str());
    InOutObj.String(InData.c_str());
}
