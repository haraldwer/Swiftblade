#version 330

#include "Shaders/Uniforms/SH_FrameUniforms.si"
#include "Shaders/Utility/SH_DeferredID.si"
#include "Shaders/Utility/SH_Noise.si"

// In
in vec4 WorldPosition;
in vec3 WorldNormal;
in vec3 ObjectPosition;
in vec3 VertexPosition;

out vec4 Output;

void main()
{
    vec3 p = normalize(VertexPosition.xyz);
    float n = CalcPerlin(p + vec3(0.0, -1.0, 0.0) * Time * 0.05);
    Output.rgb = vec3(n);
    Output.a = 1.0f;
}