#include "RenderScene.h"

#include "Instances/MeshInstance.h"

void RenderScene::Render()
{
    ClearBackground(SKYBLUE);
    BeginMode3D(Camera);
    
    // TODO: Sub-tick camera movement
    // TODO: Depth sorting
    // TODO: Merge instances
    
    // Draw primitives
    for (const MeshInstance& mesh : Meshes)
        DrawMesh(mesh.Mesh, mesh.Material, mesh.Transform);

    DrawGrid(10, 10.0f);

    EndMode3D();
}
