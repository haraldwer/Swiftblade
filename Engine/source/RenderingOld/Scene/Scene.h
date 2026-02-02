#pragma once

//#include "Culling/MeshCollection.h"
//#include "Instances/CameraInstance.h"
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
        typedef std::function<void()> CustomFunc;
        
        void Clear()
        {
            camera = {};
        }
        
        void Build()
        {
            
        }

        CameraInstance camera;
        //Vector<EnvironmentInstance> environments = {};
        //MeshCollection meshes = {};
        //Utility::SplitContainer<LightInstance> lights = {};
        //Utility::SplitContainer<ParticleInstance> particles = {};
        //Utility::SplitContainer<DebugShape> shapes = {};
        //Utility::SplitContainer<DebugLine> lines = {};
        //Vector<CustomFunc> functions;
    };
}

