#pragma once

#include "Core/Resource/Resource.h"

namespace Rendering
{
    class ModelResource : public Resource::Base
    {
        CLASS_INFO(ModelResource, Resource::Base)
    public:
        bool Load() override;
        bool Unload() override;
        
        Model* Get() const;
        float GetRange() const;
        
        uint32 Hash() const;
        bool Edit(const String &InName, uint32 InOffset = 0) override;
        
        static bool Accept(const String& InPath)
        {
            return InPath.ends_with(".obj");          
        }
        
    private:
        
        // Probably contains a set of meshes
        // And LOD levels
        // What about metadata?
        // Extend resource system to include this?
        
        uint32 cachedHash = 0;
        float range = 0.0f;

        Map<uint32, Vector<uint32>> editIndices;
    };
}

typedef Resource::Ref<Rendering::ModelResource> ResModel;  
