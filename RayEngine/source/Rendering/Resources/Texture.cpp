#include "Texture.h"

#include "Utility/File/File.h"
#include "raylib.h"

bool TextureResource::Load(const String& InIdentifier)
{
    Identifier = InIdentifier;
    if (Identifier.starts_with("Gen_"))
        return true;
    Ptr = new Texture();
    *Ptr = LoadTexture(InIdentifier.c_str());
    return Ptr->id;
}

bool TextureResource::Unload()
{
    if (Ptr)
    {
        UnloadTexture(*Ptr);
        delete (Ptr);
        Ptr = nullptr; 
        return true;
    }
    return false; 
}

Utility::Timepoint TextureResource::GetEditTime() const
{
    if (Identifier.starts_with("Gen_"))
        return {};
    return Utility::GetFileWriteTime(Identifier);  
}
