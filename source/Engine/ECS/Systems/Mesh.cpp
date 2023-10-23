#include "Mesh.h"

#include "Transform.h"
#include "Engine/Resource/ResourceManager.h"
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
    const Rendering::Mesh m {
        InComponent.Model,
        Mat4F(
            QuatF::Identity(),
            t.Position,
            Vec3F::One())
    };
    Game::Get().GetRenderScene().AddMesh(m);
}
