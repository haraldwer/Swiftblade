#version 330

#include "Shaders/Uniforms/SH_FrameUniforms.si"

uniform vec3 ProbePosition;
uniform vec3 ProbeDensity;
uniform samplerCube TexEnvCube;

// In
in vec4 WorldPosition;
in vec3 WorldNormal;
in vec3 ObjectPosition;
in vec3 VertexPosition;

out vec4 Output;

void main()
{
    vec3 diff = WorldPosition.xyz - ProbePosition;
    diff /= length(diff);
    Output.rgb = texture(TexEnvCube, diff).rgb;
    Output.a = 1.0f;
}