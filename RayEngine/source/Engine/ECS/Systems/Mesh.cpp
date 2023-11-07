#include "Mesh.h"

#include "Transform.h"
#include "Editor/BlueprintEditor.h"
#include "Game/Game.h"

using namespace ECS;

void SysMesh::Update(EntityID InEntity, Mesh& InComponent, double InDelta)
{
    // TODO: Static component optimization
    
    if (!InComponent.Visible)
        return;

    const Transform& t = Get<Transform>(InEntity);
    const MeshInstance m {
        InComponent.Model,
        InComponent.Material,
        t.Matrix()
    };

    Engine::InstanceBase::Get().GetRenderScene().AddMesh(m);
}
