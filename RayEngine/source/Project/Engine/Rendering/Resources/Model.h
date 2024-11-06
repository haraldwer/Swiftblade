#pragma once

#include "Engine/Resource/Resource.h"

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
    bool Edit(const String& InName) { return false; };

    uint32 Hash() const;
    
private:
    String Identifier; 
    Model* Ptr = nullptr; 
};

typedef Resource::Ref<ModelResource> ResModel;  
