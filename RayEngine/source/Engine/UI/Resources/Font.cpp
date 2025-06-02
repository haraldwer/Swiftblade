#include "Font.h"

#include "Utility/File/File.h"
#include "raylib.h"

bool FontResource::Load(const String& InIdentifier)
{
    identifier = InIdentifier;
    ptr = new Font();
    *ptr = LoadFontEx(InIdentifier.c_str(), 100, nullptr, 0);
    SetTextureFilter(ptr->texture, TEXTURE_FILTER_POINT);
    return true;
}

bool FontResource::Unload()
{
    if (ptr)
    {
        UnloadFont(*ptr);
        delete (ptr);
        ptr = nullptr; 
        return true;
    }
    return false; 
}

Utility::Timepoint FontResource::GetEditTime() const
{
    return Utility::GetFileWriteTime(identifier);  
}
