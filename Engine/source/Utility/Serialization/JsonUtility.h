#pragma once

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

typedef rapidjson::Writer<rapidjson::StringBuffer> SerializeObj;
typedef rapidjson::GenericValue<rapidjson::UTF8<>>::ConstObject DeserializeObj;
typedef rapidjson::GenericValue<rapidjson::UTF8<>> GenericVal; 
typedef rapidjson::Document DocumentObj;

namespace Utility
{
    inline String FormatJson(const String& InJson, bool InDoTabs = true)
    {
        String formatted;
        int offset = 0;
        int totalIndent = 0;
        bool inString = false;
        for (int i = 0; i < static_cast<int>(InJson.size()); i++)
        {
            if (InJson[i] == '\"')
            {
                if (inString)
                {
                    int j = i - 1;
                    while (j >= 0 && InJson[j] == '\\')
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
                if (InJson[i] == ',')
                {
                    // If comma and next is scope start
                    if (i + 1 < static_cast<int>(InJson.size()) && InJson[i + 1] == '{')
                        continue;
                    
                    formatted.append(InJson.substr(offset, i - offset));
                    formatted.append({ InJson[i] });
                    offset = i + 1;
                    formatted.append("\n");
                    for (int j = 0; j < totalIndent; j++)
                        if (InDoTabs) formatted.append("\t");
                }

                if (InJson[i] == '{')
                {
                    formatted.append(InJson.substr(offset, i - offset));
                    formatted.append({ InJson[i] });
                    offset = i + 1;
                    
                    totalIndent++;

                    // If next is not closer
                    if (i + 1 >= static_cast<int>(InJson.size()) || InJson[i + 1] != '}')
                    {
                        formatted.append("\n");
                        for (int j = 0; j < totalIndent; j++)
                        {
                            if (InDoTabs)
                                formatted.append("\t");
                        }
                    }
                }

                if (InJson[i] == '[')
                {
                    formatted.append(InJson.substr(offset, i - offset));
                    formatted.append({ InJson[i] });
                    offset = i + 1;
                    
                    totalIndent++;

                    // If next is not arr closer or scope start 
                    if (i + 1 >= static_cast<int>(InJson.size()) || (InJson[i + 1] != ']' && InJson[i + 1] != '{'))
                    {
                        formatted.append("\n");
                        for (int j = 0; j < totalIndent; j++)
                        {
                            if (InDoTabs)
                                formatted.append("\t");
                        }
                    }
                }
                
                if (InJson[i] == '}')
                {
                    formatted.append(InJson.substr(offset, i - offset));

                    totalIndent--;
                    
                    // If previous was not scope start
                    if (i > 0 && InJson[i - 1] != '{')
                    {
                        formatted.append("\n");
                        for (int j = 0; j < totalIndent; j++)
                            formatted.append("\t");
                    }

                    formatted.append({ InJson[i] });
                    offset = i + 1;
                }

                if (InJson[i] == ']')
                {
                    formatted.append(InJson.substr(offset, i - offset));

                    totalIndent--;
                    
                    // If previous was not scope start
                    if (i > 0 && InJson[i - 1] != '[')
                    {
                        formatted.append("\n");
                        for (int j = 0; j < totalIndent; j++)
                            formatted.append("\t");
                    }

                    formatted.append({ InJson[i] });
                    offset = i + 1;
                }
            }
        }
        return formatted;
    }
}
