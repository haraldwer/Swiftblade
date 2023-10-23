#pragma once

#include "Engine/Resource/Resource.h"

// Raylib model reference
class Model; 

class ModelResource
{
public:
    bool Load(const String& InIdentifier);
    bool Unload();
    Model* Get() const { return Ptr; }
    
private: 
    Model* Ptr = nullptr; 
};

typedef Resource<ModelResource> ResModel;  
