#pragma once

namespace Rendering
{
    namespace RHI
    {
        enum class DefaultAttributeLocation : uint8
        {
            POSITION = 0,
            TEXCOORD,
            NORMAL,
            COLOR,
            TANGENT,
            TEXCOORD2,
            INDICES,
        };
        
        int GetLocation(DefaultAttributeLocation InLocation);
        void DrawQuad();
        
        // Probably combine some of these functions...
        // Similar to DrawQuad()
        void BindVertexArray(uint32 InVAO);
        void UnbindVertexArray();
        void EnableVertexMatrixAttributes();
        uint32 LoadMatrixVertexBuffer(const Vector<Mat4F>& InMatrices);
        uint32 LoadVertexBuffer(const void *InBuffer, int InSize, bool InDynamic);
        void DisableVertexBuffer();
        void DisableVertexBufferElement();
        void UnloadVertexBuffer(uint32 InVBO);
        
        void DrawElementsInstanced(int InTriangleCount, int InNumInstances);
        void DrawInstanced(int InVertexCount, int InDrawCount);
    }
}