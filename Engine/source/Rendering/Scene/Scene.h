#pragma once

#include "Instances/CameraInstance.h"
//#include "Culling/MeshCollection.h"
//#include "Instances/DebugShape.h"
//#include "Instances/EnvironmentInstance.h"
//#include "Instances/LightInstance.h"
//#include "Instances/ParticleInstance.h"
//#include "Utility/Collections/SplitContainer.h"

namespace Rendering
{
    struct RenderArgs;
    
    struct Scene
    {
        CameraInstance camera;
        //Vector<EnvironmentInstance> environments = {};
        //MeshCollection meshes = {};
        //Utility::SplitContainer<LightInstance> lights = {};
        //Utility::SplitContainer<ParticleInstance> particles = {};
        //Utility::SplitContainer<DebugShape> shapes = {};
        //Utility::SplitContainer<DebugLine> lines = {};
        typedef std::function<void()> CustomFunc;
        //Vector<CustomFunc> functions;
        
        void Clear()
        {
            camera = {};
        }
        
        void Build()
        {
            
        }
    };
}
