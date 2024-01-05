#include "Texture.h"

#include "Utility/File/File.h"

bool TextureResource::Load(const String& InIdentifier)
{
    Identifier = InIdentifier;
    Ptr = new Texture();
    *Ptr = LoadTexture(("../Content/" + InIdentifier).c_str());
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
    return Utility::GetFileWriteTime(Identifier);  
}
