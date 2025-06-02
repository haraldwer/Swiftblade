#include "Mesh.h"

#include "Instance/Instance.h"
#include "Rendering/Scene/Instances/MeshInstance.h"
#include "Transform.h"

using namespace ECS;

void SysMesh::Frame(EntityID InID, Mesh& InComponent)
{
    System<Mesh>::Frame(InID, InComponent);
    
    // TODO: Static component optimization
    
    if (!InComponent.Visible)
        return;

    const Transform& t = Get<Transform>(InID);
    const MeshInstance m {
        .model= InComponent.Model,
        .material= InComponent.Material,
        .transform= t.World()
    };

    Engine::Instance::Get().GetRenderScene().AddMesh(m);
}
