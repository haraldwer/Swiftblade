#pragma once

#include "Culling/MeshCollection.h"
#include "Instances/CameraInstance.h"
#include "Instances/DebugShape.h"
#include "Instances/EnvironmentInstance.h"
#include "Instances/LightInstance.h"
#include "Utility/Collections/SplitContainer.h"

namespace Rendering
{
    class Scene
    {
        friend class Renderer;
        friend class Lumin;
        friend class Lights;
    public:
        void SetCamera(const CameraInstance& InCamera);
        const CameraInstance& GetCamera() const { return mainCamera; }
        
        MeshCollection& Meshes() { return meshes; }
        void AddEnvironment(const EnvironmentInstance& InEnvironment);
        void AddLight(const LightInstance& InLight);
        void AddDebugShape(const DebugShape& InShape);
        void AddDebugLine(const DebugLine& InLine);

        void Clear();
        void Build();

    private:
        CameraInstance mainCamera = {};
        Vector<EnvironmentInstance> environments = {};
        MeshCollection meshes = {};
        Utility::SplitContainer<LightInstance> lights = {};
        Utility::SplitContainer<DebugShape> debugShapes = {};
        Utility::SplitContainer<DebugLine> debugLines = {};
    };
}

