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
    // TexCoord to direction?
    vec3 p = normalize(WorldNormal.xyz);
    vec3 n = Simplex(p + Time * 0.001, p);
    Output.rgb = vec3(n);
    Output.a = 1.0f;
}