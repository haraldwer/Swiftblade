#include "Model.h"

#include "ImGui/ImPlot3D/implot3d.h"

Mesh CombineModelMeshes(const Model& model)
{
    Mesh out = { 0 };

    // First pass: count totals
    int totalVertices = 0;
    int totalIndices  = 0;

    for (int i = 0; i < model.meshCount; i++)
    {
        const Mesh& m = model.meshes[i];
        totalVertices += m.vertexCount;
        totalIndices += m.indices ? m.triangleCount * 3 : m.vertexCount;
    }

    // Allocate output buffers
    out.vertexCount   = totalVertices;
    out.triangleCount = totalIndices / 3;

    out.vertices  = (float*)MemAlloc(sizeof(float) * totalVertices * 3);
    out.normals   = (float*)MemAlloc(sizeof(float) * totalVertices * 3);
    out.texcoords = (float*)MemAlloc(sizeof(float) * totalVertices * 2);
    out.indices   = (unsigned short*)MemAlloc(sizeof(unsigned short) * totalIndices);

    int vertexOffset = 0;
    int indexOffset  = 0;

    // Second pass: copy data
    for (int m = 0; m < model.meshCount; m++)
    {
        const Mesh& src = model.meshes[m];

        // Copy vertices
        for (int v = 0; v < src.vertexCount; v++)
        {
            int dstV = vertexOffset + v;

            out.vertices[dstV * 3 + 0] = src.vertices[v * 3 + 0];
            out.vertices[dstV * 3 + 1] = src.vertices[v * 3 + 1];
            out.vertices[dstV * 3 + 2] = src.vertices[v * 3 + 2];

            if (src.normals)
            {
                out.normals[dstV * 3 + 0] = src.normals[v * 3 + 0];
                out.normals[dstV * 3 + 1] = src.normals[v * 3 + 1];
                out.normals[dstV * 3 + 2] = src.normals[v * 3 + 2];
            }

            if (src.texcoords)
            {
                out.texcoords[dstV * 2 + 0] = src.texcoords[v * 2 + 0];
                out.texcoords[dstV * 2 + 1] = src.texcoords[v * 2 + 1];
            }
        }

        if (src.indices)
        {
            int count = src.triangleCount * 3;
            for (int i = 0; i < count; i++)
                out.indices[indexOffset + i] =
                    src.indices[i] + vertexOffset;
            indexOffset += count;
        }
        else
        {
            for (int i = 0; i < src.vertexCount; i++)
                out.indices[indexOffset + i] =
                    vertexOffset + i;
            indexOffset += src.vertexCount;
        }

        vertexOffset += src.vertexCount;
    }

    UploadMesh(&out, false);
    return out;
}

Model CreateCombinedModel(const Model& InModel)
{
    Model singleMeshModel = { 0 };
    singleMeshModel.meshCount = 1;
    singleMeshModel.meshes = new Mesh();
    singleMeshModel.meshes[0] = CombineModelMeshes(InModel);
    singleMeshModel.materialCount = 1;
    singleMeshModel.materials = InModel.materials;
    return singleMeshModel;
}

bool Rendering::ModelResource::Load()
{
    ptr = new Model();
    
    Model original = LoadModel(id.Str().c_str());
    //*ptr = original.meshCount > 1 ? CreateCombinedModel(original) : original;
    *ptr = original; 
    
    cachedHash = Utility::Hash(id.Str());
    editIndices.clear();
    
    for (int i = 0; i < ptr->meshCount; i++)
    {
        auto& mesh = ptr->meshes[i];
        auto aabb = GetMeshBoundingBox(mesh);
        Vec3F dist = {
            Utility::Math::Max(abs(aabb.max.x), abs(aabb.min.x)),
            Utility::Math::Max(abs(aabb.max.y), abs(aabb.min.y)),
            Utility::Math::Max(abs(aabb.max.z), abs(aabb.min.z))
        };
        range = Utility::Math::Max(range, dist.LengthSqr());
        // TODO: Also consider offset between min and max -> Center mesh in aabb
    }
    range = Utility::Math::SquareRoot(range);
    
    return true;
}



bool Rendering::ModelResource::Unload()
{
    if (ptr)
    {
        UnloadModel(*ptr);
        delete ptr;
        ptr = nullptr;
    }
    cachedHash = 0;
    editIndices.clear();
    return true; 
}

Model* Rendering::ModelResource::Get() const
{
    if (IsModelValid(*ptr))
        return ptr;
    return nullptr;
}

float Rendering::ModelResource::GetRange() const
{
    return range;
}

uint32 Rendering::ModelResource::Hash() const
{
    return cachedHash;
}

bool Rendering::ModelResource::Edit(const String &InName, uint32 InOffset)
{
    if (!ptr)
    {
        ImGui::Text("Failed to load model");
        return false;
    }
    
    if (ImGui::CollapsingHeader(("Stats##" + Utility::ToStr(InOffset)).c_str()))
    {
        ImGui::Text("Meshes: %i", ptr->meshCount);
        ImGui::Text("Materials: %i", ptr->materialCount);
        ImGui::Text("Bones: %i", ptr->boneCount);

        for (int i = 0; i < ptr->meshCount; i++)
        {
            auto& mesh = ptr->meshes[i];
            ImGui::SeparatorText(("Mesh " + Utility::ToStr(i)).c_str());
            ImGui::Text("Vertices: %i", mesh.vertexCount);
            ImGui::Text("Triangles: %i", mesh.triangleCount);
            
            if (mesh.indices) ImGui::Text("Contains indices");
            if (mesh.texcoords) ImGui::Text("Contains texcoord");
            if (mesh.normals) ImGui::Text("Contains normals");
            if (mesh.tangents) ImGui::Text("Contains tangents");
            if (mesh.colors) ImGui::Text("Contains colors");
        }
    }
    
    ImGuiTreeNodeFlags flags = 0;
    if (InOffset == 0)
        flags |= ImGuiTreeNodeFlags_DefaultOpen;
    
    if (ImGui::CollapsingHeader(("Preview##" + Utility::ToStr(InOffset)).c_str(), flags))
    {
        float width = ImGui::GetWindowWidth();
        if (ImPlot3D::BeginPlot(Utility::GetEditName(InName, InOffset).c_str(), ImVec2(width, width), ImPlot3DFlags_NoLegend | ImPlot3DFlags_Equal | ImPlot3DFlags_NoClip | ImPlot3DFlags_NoTitle))
        {
            for (int i = 0; i < ptr->meshCount; i++)
            {
                // Choose line color
                ImVec4 line_color = ImVec4(0.5f, 0.5f, 1.0f, 1.0f);
                ImVec4 fill_color = ImVec4(0.8f, 0.8f, 1.0f, 1.0f);
                ImVec4 marker_color = ImVec4(0.2f, 0.2f, 1.0f, 1.0f);
                
                // Set fill style
                ImPlot3D::SetNextFillStyle(fill_color);
                ImPlot3D::SetNextLineStyle(line_color);
                ImPlot3D::SetNextMarkerStyle(ImPlot3DMarker_Circle, 3, marker_color, IMPLOT3D_AUTO, marker_color);
                
                auto& mesh = ptr->meshes[i];
                auto& indices = editIndices[i];
                if (indices.empty())
                {
                    if (mesh.indices)
                        for (int index = 0; index < mesh.triangleCount * 3; index++)
                            indices.push_back(mesh.indices[index]);
                    else
                        for (int index = 0; index < mesh.triangleCount * 3; index++)
                            indices.push_back(index);
                }
                
                ImPlot3D::PlotMesh(
                    Utility::GetEditName(InName, InOffset + i).c_str(),
                    reinterpret_cast<const ImPlot3DPoint*>(mesh.vertices),
                    indices.data(),
                    mesh.vertexCount,
                    indices.size(),
                    ImPlot3DMeshFlags_None);
            }
            ImPlot3D::EndPlot();
        }
    }
    
    return false;
}
