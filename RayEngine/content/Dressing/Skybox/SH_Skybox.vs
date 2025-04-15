#version 330

#include "Shaders/Uniforms/SH_FrameUniforms.si"
#include "Shaders/SH_Default_vs.si"

void main()
{
    WorldNormal = vertexNormal;
    ObjectPosition = (instanceTransform * vec4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
    WorldPosition = vec4(CameraPosition + vertexPosition * NearFar.y, 1.0f);
    vec4 screenPos = mvp * WorldPosition;
    gl_Position = DistortScreenPos(screenPos);
    WorldPosition.w = screenPos.z; // Store linear depth
}