#pragma once

#include "Core/Resource/Resource.h"
#include "Rendering/Mesh.h"

struct Model;

namespace Rendering
{
    class ModelResource : public Resource::Base
    {
        CLASS_INFO(ModelResource, Resource::Base)
    public:
        bool Load() override;
        bool Unload() override;
        bool Edit(const String &InName, uint32 InOffset = 0) override;
        
        static bool Accept(const String& InPath)
        {
            return InPath.ends_with(".obj");
        }
        
        const MeshState* GetMeshState();
        MeshLOD const* GetMesh(int InMeshIndex, int InLOD);
        int GetMeshCount();

    private:
        void ContinueLoad();
        void LoadState();
        
        bool asyncLoadStarted = false;
        int loadCount = 0;
        Vector<Mesh> meshes = {};
        MeshState state = {};
        
        // TODO: Only load requested meshes and lods
    };
    
    // TODO: Provide some default meshes...
}

typedef Resource::Ref<Rendering::ModelResource> ResModel;  
