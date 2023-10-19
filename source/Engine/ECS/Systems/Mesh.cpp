#include "Mesh.h"

#include "Game/Game.h"

using namespace ECS; 

void SysMesh::Init(EntityID InEntity, Mesh& InComponent)
{
    // Request the resources
}

void SysMesh::Update(EntityID InEntity, Mesh& InComponent, double InDelta)
{
    if (!InComponent.Visible)
        return;
        
    //Game::Get().GetRenderScene().AddMesh(instance);
}
