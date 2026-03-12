#pragma once
#include <webgpu/webgpu.h>

namespace Rendering
{
    struct MeshLOD
    {
        WGPUBuffer vertexBuffer = {};
        WGPUBuffer indexBuffer = {};
        uint32 vertexCount = 0;
        uint32 indexCount = 0;
        uint32 vertexStride = 0;
        uint32 indexStride = 0;
        WGPUIndexFormat indexFormat = WGPUIndexFormat_Uint32;
    };
    
    struct Mesh
    {
        Vector<MeshLOD> lods;
    };
    
    struct MeshState
    {
        WGPUPrimitiveState primitiveState;
        Vector<WGPUVertexBufferLayout> vertexLayouts;
        uint32 hash = 0;
    };
}
