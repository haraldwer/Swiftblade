#pragma once
#include <webgpu/webgpu.hpp>

namespace Rendering
{
    struct MeshLOD
    {
        wgpu::Buffer vertexBuffer;
        wgpu::Buffer indexBuffer;
        uint32 vertexCount = 0;
        uint32 indexCount = 0;
        uint32 vertexStride = 0;
        uint32 indexStride = 0;
        wgpu::IndexFormat indexFormat = wgpu::IndexFormat::Uint32;
    };
    
    struct Mesh
    {
        Vector<MeshLOD> lods;
    };
    
    struct MeshState
    {
        wgpu::PrimitiveState primitiveState;
        Vector<wgpu::VertexBufferLayout> vertexLayouts;
        uint32 hash = 0;
    };
}
