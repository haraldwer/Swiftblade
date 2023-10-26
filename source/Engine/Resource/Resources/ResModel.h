#pragma once

#include "..\Resource.h"

// Raylib model reference
struct Model; 

class ModelResource
{
public:
    bool Load(const String& InIdentifier);
    bool Unload();
    Model* Get() const { return Ptr; }
    
private: 
    Model* Ptr = nullptr; 
};

typedef Resource::Ref<ModelResource> ResModel;  
