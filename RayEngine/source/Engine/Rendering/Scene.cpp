#include "Scene.h"

#include "Utility/RayUtility.h"

using namespace Rendering; 

void RenderScene::Render()
{
    BeginMode3D(Utility::Ray::ConvertCamera(Cam));
    
    // TODO: Sub-tick camera movement
    // TODO: Depth sorting
    // TODO: Merge instances
    
    // Draw primitives
    for (const MeshInstance& m : Meshes)
    {
        const Matrix matrix = Utility::Ray::ConvertMat(m.Transform);

        const Material* mat = nullptr;
        if (const MaterialResource* matRsc = m.Material.Get())
            mat = matRsc->Get();
        CHECK_CONTINUE(!mat);

        if (const auto mData = m.Model.Get())
        {
            if (const auto rayModel = mData->Get())
            {
                DrawMesh(rayModel->meshes[0], *mat, matrix);
            }
        }
    }

    DrawGrid(10, 10.0f);

    EndMode3D();
}
