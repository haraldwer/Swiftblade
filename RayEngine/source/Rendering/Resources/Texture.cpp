#include "Texture.h"

#include "Utility/File/File.h"
#include "raylib.h"

bool TextureResource::Load(const String& InIdentifier)
{
    identifier = InIdentifier;
    if (identifier.starts_with("Gen_"))
        return true;
    ptr = new Texture();
    *ptr = LoadTexture(InIdentifier.c_str());
    return ptr->id;
}

bool TextureResource::Unload()
{
    if (ptr)
    {
        UnloadTexture(*ptr);
        delete (ptr);
        ptr = nullptr; 
        return true;
    }
    return false; 
}

Utility::Timepoint TextureResource::GetEditTime() const
{
    if (identifier.starts_with("Gen_"))
        return {};
    return Utility::GetFileWriteTime(identifier);  
}
