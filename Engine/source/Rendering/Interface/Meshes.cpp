#include "../../Rendering/Interface/Meshes.h"

int Rendering::RHI::GetLocation(DefaultAttributeLocation InLocation)
{
    return static_cast<int>(InLocation);
}

void Rendering::RHI::DrawQuad()
{
}

void Rendering::RHI::BindVertexArray(uint32 InVAO)
{
}

void Rendering::RHI::UnbindVertexArray()
{
}

void Rendering::RHI::EnableVertexMatrixAttributes()
{ 
}

uint32 Rendering::RHI::LoadMatrixVertexBuffer(const Vector<Mat4F> &InMatrices)
{
    return 0;
}

uint32 Rendering::RHI::LoadVertexBuffer(const void *InBuffer, int InSize, bool InDynamic)
{
    return 0;
}

void Rendering::RHI::DisableVertexBuffer()
{
}

void Rendering::RHI::DisableVertexBufferElement()
{
}

void Rendering::RHI::UnloadVertexBuffer(const uint32 InVBO)
{
}

void Rendering::RHI::DrawElementsInstanced(const int InTriangleCount, const int InNumInstances)
{
}

void Rendering::RHI::DrawInstanced(const int InVertexCount, const int InDrawCount)
{
}
