#pragma once

namespace Utility
{
    inline String StringReplace(const String& InStr, const String& InSearch, const String& InRep)
    {
        if(InSearch.empty())
            return InStr;
        
        String result = InStr;
        size_t start_pos = 0;
        while((start_pos = result.find(InSearch, start_pos)) != std::string::npos) {
            result.replace(start_pos, InSearch.length(), InRep);
            start_pos += InRep.length();
        }
        return result;
    }

    inline String StringRemove(const String& InStr, const String& InSearch)
    {
        return StringReplace(InStr, InSearch, "");        
    }
}