#include "Font.h"

#include "Utility/File/File.h"
#include "raylib.h"

bool FontResource::Load(const String& InIdentifier)
{
    Identifier = InIdentifier;
    Ptr = new Font();
    *Ptr = LoadFontEx(("../Content/" + InIdentifier).c_str(), 100, nullptr, 0);
    SetTextureFilter(Ptr->texture, TEXTURE_FILTER_POINT);
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
