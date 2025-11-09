#include "Model.h"

#include "raylib.h"
#include "ImGui/ImPlot3D/implot3d.h"

bool Rendering::ModelResource::Load()
{
    ptr = new Model(); 
    *ptr = LoadModel(id.Str().c_str());
    cachedHash = Utility::Hash(id.Str());
    editIndices.clear();
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

Model*Rendering::ModelResource::Get() const
{
    if (IsModelValid(*ptr))
        return ptr;
    return nullptr;
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
    
    if (ImPlot3D::BeginPlot(Utility::GetEditName(InName, InOffset).c_str(), ImVec2(-1, -1), ImPlot3DFlags_NoLegend | ImPlot3DFlags_Equal | ImPlot3DFlags_NoClip | ImPlot3DFlags_NoTitle))
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
    return false;
}
