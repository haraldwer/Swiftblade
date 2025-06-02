#pragma once

namespace Utility
{
    inline String StringReplace(const String& InStr, const String& InSearch, const String& InRep)
    {
        if(InSearch.empty())
            return InStr;
        
        String result = InStr;
        size_t startPos = 0;
        while((startPos = result.find(InSearch, startPos)) != std::string::npos) {
            result.replace(startPos, InSearch.length(), InRep);
            startPos += InRep.length();
        }
        return result;
    }

    inline String StringRemove(const String& InStr, const String& InSearch)
    {
        return StringReplace(InStr, InSearch, "");        
    }
}