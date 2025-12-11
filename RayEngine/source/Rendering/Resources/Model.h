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
        bool Edit(const String &InName, uint32 InOffset = 0) override;

        static bool Accept(const String& InPath)
        {
            return InPath.ends_with(".obj");          
        }
        
    private:
        uint32 cachedHash = 0;
        Model* ptr = nullptr;

        Map<uint32, Vector<uint32>> editIndices;
    };
}

typedef Resource::Ref<Rendering::ModelResource> ResModel;  
