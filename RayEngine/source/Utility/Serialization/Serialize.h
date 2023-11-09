#pragma once

namespace Utility
{
    template <class T>
    void WriteValue(SerializeObj& InOutObj, const T& InData)
    {
        InData.Serialize(InOutObj);
    }
    
    void WriteValue(SerializeObj& InOutObj, const bool& InData);
    void WriteValue(SerializeObj& InOutObj, const float& InData);
    void WriteValue(SerializeObj& InOutObj, const int32& InData);
    void WriteValue(SerializeObj& InOutObj, const Vec3F& InData);
    void WriteValue(SerializeObj& InOutObj, const QuatF& InData);
    void WriteValue(SerializeObj& InOutObj, const Mat4F& InData);
    void WriteValue(SerializeObj& InOutObj, const String& InData);
    
    template <class T>
    void WriteValue(SerializeObj& InOutObj, const Vector<T>& InData)
    {
        InOutObj.StartArray();
        for (auto& entry : InData)
            WriteValue(InOutObj, entry);
        InOutObj.EndArray();
    }
    
    template <class K, class V>
    void WriteValue(SerializeObj& InOutObj, const Map<K, V>& InData)
    {
        InOutObj.StartArray();
        for (auto& entry : InData)
        {
            InOutObj.StartObject();
            InOutObj.Key("Key");
            WriteValue(InOutObj, entry.first);
            InOutObj.Key("Val");
            WriteValue(InOutObj, entry.second);
            InOutObj.EndObject();
        }
        InOutObj.EndArray();
    }

    template <class T>
    void Serialize(SerializeObj& InOutObj, const String& InName, const T& InData)
    {
        InOutObj.Key(InName.c_str());
        WriteValue(InOutObj, InData);  
    }
}
