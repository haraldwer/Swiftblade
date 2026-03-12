#pragma once

#include "webgpu/webgpu.h"

namespace Rendering
{
    struct Command;

    class CommandList
    {
    public:
        void Begin(const String& InName);

        void RenderFullscreen(const Command &InCommand, const WGPURenderPassEncoder& renderPass);

        void RenderModel(const Command &InCommand, const WGPURenderPassEncoder& renderPass);

        void Add(const Command& InCommand);
        void End();
        void Submit();
        
    private: 
        String workingName = "";
        WGPUCommandEncoder encoder = {};
        Vector<WGPUCommandBuffer> commands;
    };
}
