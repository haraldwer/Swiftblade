#version 330

#include "Shaders/SH_Default_vs.si"

void main()
{
    WorldNormal = vertexNormal;
    VertexPosition = vertexPosition;
    ObjectPosition = (instanceTransform * vec4(vec3(0), 1)).xyz;
    WorldPosition = vec4(CameraPosition - vertexPosition, 1);
    vec4 screenPos = ViewProj * WorldPosition;
    gl_Position = screenPos;
    WorldPosition.w = screenPos.z; // Store linear depth
    TexCoord = vertexTexCoord;
}