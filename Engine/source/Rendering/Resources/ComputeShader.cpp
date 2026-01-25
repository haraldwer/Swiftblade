#include "ComputeShader.h"

bool Rendering::ComputeShaderResourceBase::Load()
{
    return true;
}

void Rendering::ComputeShaderResourceBase::Dispatch(void *InPtr, int InSize)
{
}

void Rendering::ComputeShaderResourceBase::Collect(void *InPtr, int &InSize)
{
}
