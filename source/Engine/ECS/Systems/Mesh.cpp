#include "Mesh.h"

#include "Transform.h"
#include "Game/Game.h"

using namespace ECS;

void SysMesh::Init(EntityID InEntity, Mesh& InComponent)
{
    // Request the resources
    InComponent.Model = ResModel("../content/test.obj");
}

void SysMesh::Update(EntityID InEntity, Mesh& InComponent, double InDelta)
{
    if (!InComponent.Visible)
        return;

    const Transform& t = GetSystem<SysTransform>().Get(InEntity);
    const MeshInstance m {
        InComponent.Model,
        t.Matrix()
    };
    Game::Get().GetRenderScene().AddMesh(m);
}
