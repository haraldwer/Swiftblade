#include "Mesh.h"

#include "Instance/Instance.h"
#include "Rendering/Instances/MeshInstance.h"
#include "Transform.h"

using namespace ECS;

void SysMesh::Frame(EntityID InEntity, Mesh& InComponent)
{
    // TODO: Static component optimization
    
    if (!InComponent.Visible)
        return;

    const Transform& t = Get<Transform>(InEntity);
    const MeshInstance m {
        InComponent.Model,
        InComponent.Material,
        t.World()
    };

    Engine::Instance::Get().GetRenderScene().AddMesh(m);
}