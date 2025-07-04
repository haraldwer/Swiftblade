#include "Scene.h"

using namespace Rendering;

void Scene::Clear()
{
    PROFILE_GL();
    meshes.ClearNonPersistent();
    environments = {};
    lights.Clear();
    debugShapes.Clear();
    debugLines.Clear();
}

void Scene::Build()
{
    PROFILE_GL();
    meshes.Build();
    lights.Build();
    debugShapes.Build();
    debugLines.Build();
}

void Scene::SetCamera(const CameraInstance& InCamera)
{
    mainCamera = InCamera;
}

void Scene::AddEnvironment(const EnvironmentInstance& InEnvironment)
{
    environments.push_back(InEnvironment);
}

void Scene::AddLight(const LightInstance& InLight)
{
    lights.Insert(InLight, {
        .position = InLight.data.position,
        .extent = InLight.data.range
    }); 
}

void Scene::AddDebugShape(const DebugShape& InShape)
{
    debugShapes.Insert(InShape,{
        .position = InShape.pos,
        .extent = InShape.data.r
    });
}

void Scene::AddDebugLine(const DebugLine& InLine)
{
    const Vec3F diff = (InLine.end - InLine.start)/2; 
    debugLines.Insert(InLine,{
        .position = diff,
        .extent = diff.Length()
    });
}
