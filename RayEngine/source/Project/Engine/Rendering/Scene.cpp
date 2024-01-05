#include "Scene.h"

#include "raymath.h"
#include "rlgl.h"
#include "Utility/Hash.h"
#include "Utility/RayUtility.h"

using namespace Rendering;

void MeshCollection::AddMesh(const MeshInstance& InInstance)
{
    auto& entry = GetEntry(InInstance);
    entry.Transforms.push_back(InInstance.Transform);
}

void MeshCollection::AddMeshes(const MeshInstance& InInstance, const Vector<Mat4F>& InTransforms)
{
    Entry& entry = GetEntry(InInstance);
    entry.Transforms.reserve(entry.Transforms.size() + InTransforms.size());
    entry.Transforms.insert(entry.Transforms.end(), InTransforms.begin(), InTransforms.end());
}

MeshCollection::Entry& MeshCollection::GetEntry(const MeshInstance& InInstance)
{
    const String matID = InInstance.Material.Identifier();
    const String modelID = InInstance.Model.Identifier();

    union Union
    {
        struct Res
        {
            uint32 Material;
            uint32 Model;
        } Resource;
        uint64 Key = 0;
    } hash;
    
    hash.Resource.Material = Utility::Hash(matID); 
    hash.Resource.Model = Utility::Hash(modelID);
    
    Entry& entry = Entries[hash.Key];
    if (!entry.Initialized)
    {
        entry.Material = InInstance.Material;
        entry.Model = InInstance.Model;
        entry.Initialized = true; 
    }
    return entry;
}

void RenderScene::Render()
{
    BeginMode3D(Utility::Ray::ConvertCamera(Cam));
    
    // TODO: Sub-tick camera movement
    // TODO: Depth sorting

    // Instanced rendering
    for (auto& entry : Meshes.Entries)
    {
        const Mesh* meshes = nullptr;
        int32 meshCount = 0;
        if (const auto resModel = entry.second.Model.Get())
        {
            if (const auto rlModel = resModel->Get())
            {
                if (rlModel->meshCount > 0)
                {
                    meshes = rlModel->meshes;
                    meshCount = rlModel->meshCount;
                }
            }
        }

        const Shader* shader = nullptr; 
        if (const auto resMat = entry.second.Material.Get())
        {
            if (const auto resShader = resMat->Shader.Get().Get())
                shader =resShader->Get();
            resMat->TwoSided ?
                rlDisableBackfaceCulling() :
                rlEnableBackfaceCulling();
        }

        CHECK_CONTINUE(!meshes);
        CHECK_CONTINUE(meshCount == 0);
        CHECK_CONTINUE(!shader);
        CHECK_CONTINUE(entry.second.Transforms.empty());
        
        for (int i = 0; i < meshCount; i++)
            DrawInstances(meshes[i], *shader, entry.second.Transforms);
    }
    
    for (auto& shape : DebugShapes)
    {
        switch (shape.Type)
        {
        case DebugShapeInstance::Type::SPHERE:
            DrawSphereWires(
                Utility::Ray::ConvertVec(shape.Pos),
                shape.Data.x,
                static_cast<int>(shape.Data.y),
                static_cast<int>(shape.Data.z),
                shape.Color);
            break;
        case DebugShapeInstance::Type::BOX:
            DrawCubeWiresV(
                Utility::Ray::ConvertVec(shape.Pos),
                Utility::Ray::ConvertVec(shape.Data),
                shape.Color);
            break;
        case DebugShapeInstance::Type::CAPSULE:
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

    for (auto& line : DebugLines)
        DrawLine3D(
            Utility::Ray::ConvertVec(line.Start),
            Utility::Ray::ConvertVec(line.End),
            line.Color);

    EndMode3D();
}

void RenderScene::DrawInstances(const Mesh& InMesh, const Shader& InShader, const Vector<Mat4F>& InMatrices) const
{
    int instances = static_cast<int>(InMatrices.size());
    
    // Send required data to shader (matrices, values)
    //-----------------------------------------------------
    rlEnableShader(InShader.id);
        
    const int cameraPos = GetShaderLocation(InShader, "cameraPosition");
    SetShaderValue(InShader, cameraPos, Cam.Position.data, SHADER_UNIFORM_VEC3);
    
    // Get a copy of current matrices to work with,
    // just in case stereo render is required, and we need to modify them
    // NOTE: At this point the modelview matrix just contains the view matrix (camera)
    // That's because BeginMode3D() sets it and there is no model-drawing function
    // that modifies it, all use rlPushMatrix() and rlPopMatrix()
    Matrix matModel = MatrixIdentity();
    Matrix matView = rlGetMatrixModelview();
    Matrix matModelView = MatrixIdentity();
    Matrix matProjection = rlGetMatrixProjection();
    
    // Upload view and projection matrices (if locations available)
    if (InShader.locs[SHADER_LOC_MATRIX_VIEW] != -1)
        rlSetUniformMatrix(InShader.locs[SHADER_LOC_MATRIX_VIEW], matView);
    if (InShader.locs[SHADER_LOC_MATRIX_PROJECTION] != -1)
        rlSetUniformMatrix(InShader.locs[SHADER_LOC_MATRIX_PROJECTION], matProjection);
        
    // Enable mesh VAO to attach new buffer
    rlEnableVertexArray(InMesh.vaoId);

    // This could alternatively use a static VBO and either glMapBuffer() or glBufferSubData().
    // It isn't clear which would be reliably faster in all cases and on all platforms,
    // anecdotally glMapBuffer() seems very slow (syncs) while glBufferSubData() seems
    // no faster, since we're transferring all the transform matrices anyway
    unsigned int instancesVboId = 0;
    int bufferSize = instances * sizeof(Mat4F);
    instancesVboId = rlLoadVertexBuffer(InMatrices.data(), bufferSize, false);

    // Instances transformation matrices are send to shader attribute location: SHADER_LOC_MATRIX_MODEL
    for (unsigned int i = 0; i < 4; i++)
    {
        rlEnableVertexAttribute(InShader.locs[SHADER_LOC_MATRIX_MODEL] + i);
        rlSetVertexAttribute(InShader.locs[SHADER_LOC_MATRIX_MODEL] + i, 4, RL_FLOAT, 0, sizeof(Mat4F), (void *)(i*sizeof(Vec4F)));
        rlSetVertexAttributeDivisor(InShader.locs[SHADER_LOC_MATRIX_MODEL] + i, 1);
    }

    rlDisableVertexBuffer();
    rlDisableVertexArray();

    // Accumulate internal matrix transform (push/pop) and view matrix
    // NOTE: In this case, model instance transformation must be computed in the shader
    matModelView = MatrixMultiply(rlGetMatrixTransform(), matView);

    // Upload model normal matrix (if locations available)
    if (InShader.locs[SHADER_LOC_MATRIX_NORMAL] != -1)
        rlSetUniformMatrix(InShader.locs[SHADER_LOC_MATRIX_NORMAL], MatrixTranspose(MatrixInvert(matModel)));
    //-----------------------------------------------------

    // Try binding vertex array objects (VAO)
    // or use VBOs if not possible
    if (!rlEnableVertexArray(InMesh.vaoId))
    {
        // Bind mesh VBO data: vertex position (shader-location = 0)
        rlEnableVertexBuffer(InMesh.vboId[0]);
        rlSetVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_POSITION], 3, RL_FLOAT, 0, 0, 0);
        rlEnableVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_POSITION]);

        // Bind mesh VBO data: vertex texcoords (shader-location = 1)
        rlEnableVertexBuffer(InMesh.vboId[1]);
        rlSetVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_TEXCOORD01], 2, RL_FLOAT, 0, 0, 0);
        rlEnableVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_TEXCOORD01]);

        if (InShader.locs[SHADER_LOC_VERTEX_NORMAL] != -1)
        {
            // Bind mesh VBO data: vertex normals (shader-location = 2)
            rlEnableVertexBuffer(InMesh.vboId[2]);
            rlSetVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_NORMAL], 3, RL_FLOAT, 0, 0, 0);
            rlEnableVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_NORMAL]);
        }

        // Bind mesh VBO data: vertex colors (shader-location = 3, if available)
        if (InShader.locs[SHADER_LOC_VERTEX_COLOR] != -1)
        {
            if (InMesh.vboId[3] != 0)
            {
                rlEnableVertexBuffer(InMesh.vboId[3]);
                rlSetVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_COLOR], 4, RL_UNSIGNED_BYTE, 1, 0, 0);
                rlEnableVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_COLOR]);
            }
            else
            {
                // Set default value for unused attribute
                // NOTE: Required when using default shader and no VAO support
                float value[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                rlSetVertexAttributeDefault(InShader.locs[SHADER_LOC_VERTEX_COLOR], value, SHADER_ATTRIB_VEC4, 4);
                rlDisableVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_COLOR]);
            }
        }

        // Bind mesh VBO data: vertex tangents (shader-location = 4, if available)
        if (InShader.locs[SHADER_LOC_VERTEX_TANGENT] != -1)
        {
            rlEnableVertexBuffer(InMesh.vboId[4]);
            rlSetVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_TANGENT], 4, RL_FLOAT, 0, 0, 0);
            rlEnableVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_TANGENT]);
        }

        // Bind mesh VBO data: vertex texcoords2 (shader-location = 5, if available)
        if (InShader.locs[SHADER_LOC_VERTEX_TEXCOORD02] != -1)
        {
            rlEnableVertexBuffer(InMesh.vboId[5]);
            rlSetVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_TEXCOORD02], 2, RL_FLOAT, 0, 0, 0);
            rlEnableVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_TEXCOORD02]);
        }

        if (InMesh.indices != NULL)
            rlEnableVertexBufferElement(InMesh.vboId[6]);
    }

    // WARNING: Disable vertex attribute color input if mesh can not provide that data (despite location being enabled in shader)
    if (InMesh.vboId[3] == 0)
        rlDisableVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_COLOR]);

    int eyeCount = rlIsStereoRenderEnabled() ? 2 : 1;
    for (int eye = 0; eye < eyeCount; eye++)
    {
        // Calculate model-view-projection matrix (MVP)
        Matrix matModelViewProjection = MatrixIdentity();
        if (eyeCount == 1) matModelViewProjection = MatrixMultiply(matModelView, matProjection);
        else
        {
            // Setup current eye viewport (half screen width)
            rlViewport(eye*rlGetFramebufferWidth()/2, 0, rlGetFramebufferWidth()/2, rlGetFramebufferHeight());
            matModelViewProjection = MatrixMultiply(MatrixMultiply(matModelView, rlGetMatrixViewOffsetStereo(eye)), rlGetMatrixProjectionStereo(eye));
        }

        // Send combined model-view-projection matrix to shader
        rlSetUniformMatrix(InShader.locs[SHADER_LOC_MATRIX_MVP], matModelViewProjection);

        // Draw mesh instanced
        if (InMesh.indices != NULL)
            rlDrawVertexArrayElementsInstanced(0, InMesh.triangleCount*3, 0, instances);
        else
            rlDrawVertexArrayInstanced(0, InMesh.vertexCount, instances);
    }

    // Disable all possible vertex array objects (or VBOs)
    rlDisableVertexArray();
    rlDisableVertexBuffer();
    rlDisableVertexBufferElement();

    // Disable shader program
    rlDisableShader();

    // Remove instance transforms buffer
    rlUnloadVertexBuffer(instancesVboId);
}
