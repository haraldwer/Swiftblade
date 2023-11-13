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

    for (auto& shape : DebugShapes)
    {
        switch (shape.Type)
        {
        case DebugShape::Type::SPHERE:
            DrawSphereWires(
                Utility::Ray::ConvertVec(shape.Pos),
                shape.Data.x,
                static_cast<int>(shape.Data.y),
                static_cast<int>(shape.Data.z),
                shape.Color);
            break;
        case DebugShape::Type::BOX:
            DrawCubeWiresV(
                Utility::Ray::ConvertVec(shape.Pos),
                Utility::Ray::ConvertVec(shape.Data),
                shape.Color);
            break;
        case DebugShape::Type::CAPSULE:
            const Vec3F dir = Mat4F(shape.Rot).Right() * shape.Data.y;
            const auto start = Utility::Ray::ConvertVec(shape.Pos + dir);
            const auto end = Utility::Ray::ConvertVec(shape.Pos - dir);
            DrawCapsuleWires(
                start,
                end,
                shape.Data.x,
                static_cast<int>(shape.Data.z),
                static_cast<int>(shape.Data.z) / 2,
                shape.Color);
            break;
        }
    }

    EndMode3D();
}
