#include "Shaders/Uniforms/SH_FrameUniforms.si"
#include "Shaders/Utility/SH_CubeSampling.si"

uniform vec3 Position;
uniform vec3 Direction;
uniform vec3 Color;
uniform float Range;
uniform float ConeRadius;
uniform float Intensity;

uniform float Timestamp;
uniform vec3 ShadowPosition;
uniform vec4 ShadowRect;
uniform sampler2D TexShadow;
uniform vec2 FaceTexel;

// In
in vec4 WorldPosition;
in vec3 WorldNormal;
in vec3 ObjectPosition;
in vec3 VertexPosition;
in vec2 TexCoord;

out vec4 Output;

void main()
{
    vec3 diff = normalize(ObjectPosition - WorldPosition.xyz);
    Output = SampleCubeAtlas(TexShadow, FaceTexel, ShadowRect, -diff);
}