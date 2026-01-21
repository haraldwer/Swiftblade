#pragma once
#include <algorithm>

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

    inline void StringSplit(String InStr, const String& InDelimiter, const std::function<void(const String&)>& InFunc)
    {
        std::size_t pos = InStr.find(InDelimiter);
        while (pos != std::string::npos)
        {
            InFunc(InStr.substr(0, pos));
            InStr.erase(0, pos + InDelimiter.length());
            pos = InStr.find(InDelimiter);
        }
        if (!InStr.empty())
            InFunc(InStr);
    }

    inline Vector<String> StringSplit(const String& InStr, const String& InDelimiter)
    {
        Vector<String> result;
        StringSplit(InStr, InDelimiter, [&](const String& InResult) { result.push_back(InResult); });
        return result;
    }
    
    inline String ToUpper(String InStr)
    {
        std::ranges::transform(InStr.begin(), InStr.end(), InStr.begin(), toupper);
        return InStr;
    }
}
