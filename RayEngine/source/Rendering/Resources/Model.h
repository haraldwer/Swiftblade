#pragma once

#include "Core/Resource/Resource.h"

// Raylib model reference
struct Model; 

namespace Rendering
{
    class ModelResource : public Resource::Base
    {
        CLASS_INFO(ModelResource, Resource::Base)
    public:
        bool Load() override;
        bool Unload() override;
        Model* Get() const;
        uint32 Hash() const;
        
    private:
        uint32 cachedHash = 0;
        Model* ptr = nullptr; 
    };

}

typedef Resource::Ref<Rendering::ModelResource> ResModel;  
