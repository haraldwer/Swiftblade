#include "CubeVolumeDresser.h"

#include "Engine/Instance/Instance.h"

void CubeVolumeDresser::Dress(const ECS::CubeVolume& InVolume)
{
    VisualData.Get().FlatWall

    // Iterate over volume
    // With an offset, check all
}

void CubeVolumeDresser::Draw()
{
    for (auto& mesh : Instances)
        Engine::InstanceBase::Get().GetRenderScene().AddMeshes(
            mesh.Mesh,
            mesh.Instances);  
}
