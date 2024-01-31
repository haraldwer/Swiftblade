#include "Mesh.h"

#include "Transform.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Rendering/Instances/MeshInstance.h"

using namespace ECS;

void SysMesh::Frame(EntityID InEntity, Mesh& InComponent, double InDelta)
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
