#pragma once

namespace Utility
{
    template <class T>
    String Serialize(const String& InName, T InData)
    {
        return InData.Serialize(InName); 
    }

    inline String Serialize(const String& InName, float InData)
    {
        return ""; 
    }

    template <class T>
    void Deserialize(const String& InContent, const String& InName, T& OutData)
    {
        OutData.Deserialize(InContent, InName); 
    }

    inline void Deserialize(const String& InContent, const String& InName, float& OutData)
    {
        
    }
}
