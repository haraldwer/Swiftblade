#include "Shaders/Uniforms/SH_FrameUniforms.si"
#include "Shaders/Utility/SH_CubeSampling.si"

uniform float Timestamp;
uniform vec3 ProbePosition;
uniform vec3 ProbeRange;
uniform vec4 ProbeRect;
uniform sampler2D TexLumin;

// In
in vec4 WorldPosition;
in vec3 WorldNormal;
in vec3 ObjectPosition;
in vec3 VertexPosition;
in vec2 TexCoord;

out vec4 Output;

void main()
{
    vec3 diff = normalize(ProbePosition - WorldPosition.xyz);
    Output.rgb = SampleCubeAtlas(TexLumin, ProbeRect, -diff).rgb;
    Output.a = 1.0f;
}