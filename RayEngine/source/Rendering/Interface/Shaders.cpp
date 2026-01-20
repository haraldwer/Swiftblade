#include "Shaders.h"

void Rendering::RHI::SetUniform(int InLocation, const Mat4F* InValue, int InCount)
{
    CHECK_ASSERT(!InValue, "Invalid ptr");
    glUniformMatrix4fv(InLocation, InCount, false, InValue->data);    
}

void Rendering::RHI::SetUniform(int InLocation, const Vec4F* InValue, int InCount)
{
    CHECK_ASSERT(!InValue, "Invalid ptr");
    glUniform4fv(InLocation, InCount, InValue->data);
}

void Rendering::RHI::SetUniform(int InLocation, const Vec3F* InValue, int InCount)
{
    CHECK_ASSERT(!InValue, "Invalid ptr");
    glUniform3fv(InLocation, InCount, InValue->data);
}

void Rendering::RHI::SetUniform(int InLocation, const Vec2F* InValue, int InCount)
{
    CHECK_ASSERT(!InValue, "Invalid ptr");
    glUniform2fv(InLocation, InCount, InValue->data);
}

void Rendering::RHI::SetUniform(int InLocation, const float* InValue, int InCount)
{
    CHECK_ASSERT(!InValue, "Invalid ptr");
    glUniform1fv(InLocation, InCount, InValue);
}

void Rendering::RHI::SetUniform(int InLocation, const int* InValue, int InCount)
{
    CHECK_ASSERT(!InValue, "Invalid ptr");
    glUniform1iv(InLocation, InCount, InValue);
}

void Rendering::RHI::SetUniform(int InLocation, UniformType InType, const void *InValue, int InCount)
{
    CHECK_ASSERT(!InValue, "Invalid ptr");
    switch (InType)
    {
        case UniformType::FLOAT: glUniform1fv(InLocation, InCount, (float *)InValue); break;
        case UniformType::VEC2: glUniform2fv(InLocation, InCount, (float *)InValue); break;
        case UniformType::VEC3: glUniform3fv(InLocation, InCount, (float *)InValue); break;
        case UniformType::VEC4: glUniform4fv(InLocation, InCount, (float *)InValue); break;
        case UniformType::INT: glUniform1iv(InLocation, InCount, (int *)InValue); break;
        case UniformType::IVEC2: glUniform2iv(InLocation, InCount, (int *)InValue); break;
        case UniformType::IVEC3: glUniform3iv(InLocation, InCount, (int *)InValue); break;
        case UniformType::IVEC4: glUniform4iv(InLocation, InCount, (int *)InValue); break;
        case UniformType::MAT_4X4: glUniformMatrix4fv(InLocation, InCount, false, (float *)InValue); break;
        default: CHECK_ASSERT(false, "Unknown uniform type");
    }
}
