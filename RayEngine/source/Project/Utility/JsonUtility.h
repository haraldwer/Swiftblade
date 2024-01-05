#pragma once

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"

typedef rapidjson::Writer<rapidjson::StringBuffer> SerializeObj;
typedef rapidjson::GenericValue<rapidjson::UTF8<>>::ConstObject DeserializeObj;
typedef rapidjson::GenericValue<rapidjson::UTF8<>> GenericVal; 
typedef rapidjson::Document DocumentObj;

namespace Utility
{
    inline String FormatJson(const String& aJson, bool aDoTabs = true)
    {
        String formatted;
        int offset = 0;
        int totalIndent = 0;
        bool inString = false;
        for (int i = 0; i < aJson.size(); i++)
        {
            if (aJson[i] == '\"')
            {
                if (inString)
                {
                    int j = i - 1;
                    while (j >= 0 && aJson[j] == '\\')
                    {
                        --j;
                    }
                    if (((i - 1) - j) % 2 == 0)
                    {
                        inString = false;
                    }
                }
                else
                {
                    inString = true;
                }
            }
            if (!inString)
            {
                if (aJson[i] == ',')
                {
                    // If comma and next is scope start
                    if (i + 1 < aJson.size() && aJson[i + 1] == '{')
                        continue;
                    
                    formatted.append(aJson.substr(offset, i - offset));
                    formatted.append({ aJson[i] });
                    offset = i + 1;
                    formatted.append("\n");
                    for (int j = 0; j < totalIndent; j++)
                        if (aDoTabs) formatted.append("\t");
                }

                if (aJson[i] == '{')
                {
                    formatted.append(aJson.substr(offset, i - offset));
                    formatted.append({ aJson[i] });
                    offset = i + 1;
                    
                    totalIndent++;

                    // If next is not closer
                    if (i + 1 >= aJson.size() || aJson[i + 1] != '}')
                    {
                        formatted.append("\n");
                        for (int j = 0; j < totalIndent; j++)
                        {
                            if (aDoTabs)
                                formatted.append("\t");
                        }
                    }
                }

                if (aJson[i] == '[')
                {
                    formatted.append(aJson.substr(offset, i - offset));
                    formatted.append({ aJson[i] });
                    offset = i + 1;
                    
                    totalIndent++;

                    // If next is not arr closer or scope start 
                    if (i + 1 >= aJson.size() || (aJson[i + 1] != ']' && aJson[i + 1] != '{'))
                    {
                        formatted.append("\n");
                        for (int j = 0; j < totalIndent; j++)
                        {
                            if (aDoTabs)
                                formatted.append("\t");
                        }
                    }
                }
                
                if (aJson[i] == '}')
                {
                    formatted.append(aJson.substr(offset, i - offset));

                    totalIndent--;
                    
                    // If previous was not scope start
                    if (i > 0 && aJson[i - 1] != '{')
                    {
                        formatted.append("\n");
                        for (int j = 0; j < totalIndent; j++)
                            formatted.append("\t");
                    }

                    formatted.append({ aJson[i] });
                    offset = i + 1;
                }

                if (aJson[i] == ']')
                {
                    formatted.append(aJson.substr(offset, i - offset));

                    totalIndent--;
                    
                    // If previous was not scope start
                    if (i > 0 && aJson[i - 1] != '[')
                    {
                        formatted.append("\n");
                        for (int j = 0; j < totalIndent; j++)
                            formatted.append("\t");
                    }

                    formatted.append({ aJson[i] });
                    offset = i + 1;
                }
            }
        }
        return formatted;
    }
}
