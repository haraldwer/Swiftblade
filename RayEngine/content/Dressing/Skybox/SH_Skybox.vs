#version 330

#include "Shaders/SH_Default_vs.si"

void main()
{
    WorldNormal = vertexNormal;
    VertexPosition = vertexPosition * NearFar.y;
    ObjectPosition = (instanceTransform * vec4(vec3(0), 1)).xyz;
    WorldPosition = vec4(CameraPosition - vertexPosition, 1);
    vec4 screenPos = ViewProj * WorldPosition;
    vec4 prevScreenPos = ViewProjPrev * WorldPosition;
    gl_Position = screenPos;
    WorldPosition.w = screenPos.z; // Store linear depth
    TexCoord = vertexTexCoord;

    screenPos.xy /= screenPos.z;
    prevScreenPos.xy /= prevScreenPos.z;
    screenPos.xy = clamp(screenPos.xy, -1, 1);
    prevScreenPos.xy = clamp(prevScreenPos.xy, -1, 1);
    Velocity.xy = vec3(screenPos.xy - prevScreenPos.xy, 0);
}