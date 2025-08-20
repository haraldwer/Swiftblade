#include "Font.h"

#include "Utility/File/File.h"
#include "raylib.h"

bool FontResource::Load(const String& InIdentifier)
{
    identifier = InIdentifier;
    
    return true;
}

bool FontResource::Unload()
{
    for (auto& ptr : sizes)
    {
        if (ptr.second)
        {
            UnloadFont(*ptr.second);
            delete (ptr.second);
        }
    }
    sizes.clear();
    return true;
}

Utility::Timepoint FontResource::GetEditTime() const
{
    return Utility::GetFileWriteTime(identifier);  
}

Font* FontResource::Get(const uint32 InSize)
{
    CHECK_ASSERT(InSize == 0, "Invalid size");
    if (sizes.contains(InSize))
        return sizes.at(InSize);
    
    auto& f = sizes[InSize];
    f = new Font();
    *f = LoadFontEx(identifier.c_str(), static_cast<int>(InSize), nullptr, 0);
    SetTextureFilter(f->texture, TEXTURE_FILTER_POINT);
    return f;
}
