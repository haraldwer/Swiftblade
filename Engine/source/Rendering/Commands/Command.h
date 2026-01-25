#pragma once
#include "Resources/Material.h"
#include "Resources/Model.h"

namespace Rendering
{
    class RenderTarget;
    
    struct Command
    {
        explicit Command(const String& InName) : name(InName) {} 
        
        String name;
        
        // Where to?
        Vector<RenderTarget*> targets = {}; 
        RenderTarget* depthTarget = nullptr;
        
        // How is it gonna get there?
        ResRM material = {};
        
        // What is it?
        Vector<Mat4F> transforms = {};
        ResModel model = {};
        int uniforms = 0;
    };
}
