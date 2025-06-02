#pragma once

#include "Core/Resource/Resource.h"

// Raylib model reference
struct Model; 

class ModelResource
{
public:
    bool Load(const String& InIdentifier);
    bool Unload();
    Utility::Timepoint GetEditTime() const;
    Model* Get() const { return Ptr; }

    bool Save(const String& InPath) { return false; };
    bool Edit(const String& InName, uint32 InOffset = 0) { return false; };

    uint32 Hash() const;
    
private:
    uint32 CachedHash = 0;
    String Identifier = {};
    Model* Ptr = nullptr; 
};

typedef Resource::Ref<ModelResource> ResModel;  
