#include "Font.h"

#include "Utility/File.h"

bool FontResource::Load(const String& InIdentifier)
{
    Identifier = InIdentifier;
    Ptr = new Font();
    *Ptr = LoadFont(("../Content/" + InIdentifier).c_str());
    return Ptr->glyphCount;
}

bool FontResource::Unload()
{
    if (Ptr)
    {
        UnloadFont(*Ptr);
        delete (Ptr);
        Ptr = nullptr; 
        return true;
    }
    return false; 
}

Utility::Timepoint FontResource::GetEditTime() const
{
    return Utility::GetFileWriteTime(Identifier);  
}
