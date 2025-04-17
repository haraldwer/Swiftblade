#version 330

#include "Shaders/Uniforms/SH_FrameUniforms.si"
#include "Shaders/Utility/SH_Utility.si"

uniform vec3 ProbePosition;
uniform vec3 ProbeDensity;
uniform sampler2D TexEnvOct;

// In
in vec4 WorldPosition;
in vec3 WorldNormal;
in vec3 ObjectPosition;
in vec3 VertexPosition;
in vec2 TexCoord;

out vec4 Output;

void main()
{
    vec3 diff = WorldPosition.xyz - ProbePosition;
    Output.rgb = SampleOct(TexEnvOct, diff).rgb;
    Output.rgb = texture(TexEnvOct, TexCoord).rgb;
    //Output.rg = TexCoord;
    //Output.rgb = diff;
    Output.a = 1.0f;
}