#pragma once

namespace Rendering
{
    enum class UniformType : uint8
    {
        FLOAT,
        VEC2,
        VEC3,
        VEC4,
        INT,
        IVEC2,
        IVEC3,
        IVEC4,
        MAT_4X4,
    };
    
    namespace RHI
    {
        void SetUniform(int InLocation, const Mat4F* InValue, int InCount = 1);
        void SetUniform(int InLocation, const Vec4F* InValue, int InCount = 1);
        void SetUniform(int InLocation, const Vec3F* InValue, int InCount = 1);
        void SetUniform(int InLocation, const Vec2F* InValue, int InCount = 1);
        void SetUniform(int InLocation, const Vec2I* InValue, int InCount = 1);
        void SetUniform(int InLocation, const float* InValue, int InCount = 1);
        void SetUniform(int InLocation, const int* InValue, int InCount = 1);
        
        void SetUniform(int InLocation, UniformType InType, const void* InValue, int InCount = 1);
    }
}