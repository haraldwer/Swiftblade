#pragma once

namespace Utility
{
    template <class T>
    String ToStr(const T& InValue)
    {
        return std::to_string(InValue);
    }
    
    inline String ToStr(const String& InValue)
    {
        return InValue;
    }
    
    inline String ToStr(const char* InValue)
    {
        return { InValue };
    }

}
