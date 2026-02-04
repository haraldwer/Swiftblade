#include "Shaders/Uniforms/SH_FrameUniforms.si"
#include "Shaders/Utility/SH_Utility.si"
#include "Shaders/Utility/SH_CubeSampling.si"

uniform float Timestamp;
uniform vec3 ProbePosition;
uniform vec3 ProbeRange;
uniform vec4 ProbeRect;
uniform vec2 LuminTexel;

uniform sampler2D TexIrradiance;
uniform sampler2D TexPrefilter;

// In
in vec4 WorldPosition;
in vec3 WorldNormal;
in vec3 ObjectPosition;
in vec3 VertexPosition;
in vec2 TexCoord;

layout (location = 0) out vec4 Output;

void main()
{
    vec3 diff = Normalize(ProbePosition - WorldPosition.xyz);
    Output.rgb = SampleCubeAtlas(TexIrradiance, LuminTexel, ProbeRect, -diff).rgb;
    Output.a = 1.0f;
}