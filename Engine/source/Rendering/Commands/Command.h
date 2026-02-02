#pragma once
#include "Resources/Material.h"
#include "Resources/Model.h"

namespace Rendering
{
    class BufferCollection;
    class RenderTarget;
    typedef std::function<void(const wgpu::RenderPassEncoder& InEncoder)> RenderFunc;
    
    struct Command
    {
        explicit Command(const String& InName) : name(InName) {} 
        
        String name;
        
        // Where to?
        Vector<RenderTarget*> targets = {}; 
        RenderTarget* depthTarget = nullptr;
        bool clear = false;
        bool clearDepth = false;
        bool writeDepth = true;
        Vec4F clearColor = Vec4F::Zero();
        
        // How is it gonna get there?
        ResRM material = {};
        BufferCollection* buffers = nullptr;
        
        // What is it?
        Vector<Mat4F> transforms = {};
        ResModel model = {};
        int uniforms = 0;
        RenderFunc customFunc;
    };
}
