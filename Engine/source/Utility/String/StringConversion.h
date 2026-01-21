#pragma once

#include <string>

namespace Utility
{
    inline String ToStr(const String& InValue)
    {
        return InValue;
    }
    
    inline String ToStr(const char* InValue)
    {
        return { InValue };
    }
    
    template <class T>
    String ToStr(const T& InValue)
    {
        return std::to_string(InValue);
    }
}
