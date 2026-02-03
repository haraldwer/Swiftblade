#pragma once

#include <webgpu/webgpu.hpp>

namespace Rendering
{
    struct Command;

    class CommandList
    {
    public:
        void Begin(const String& InName);
        void Add(const Command& InCommand);
        void End();
        void Submit();
        
    private: 
        String workingName = "";
        wgpu::CommandEncoder encoder;
        Vector<wgpu::CommandBuffer> commands;
    };
}
