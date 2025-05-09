#include "Shaders/SH_Default_vs.si"

void main()
{
    WorldNormal = vertexNormal;
    vec3 vertPos = normalize(vertexPosition) * (NearFar.y - NearFar.x);
    VertexPosition = vertPos;
    ObjectPosition.xyz = CameraPosition.xyz;
    WorldPosition = vec4(CameraPosition - vertPos, 1);
    vec4 screenPos = ViewProj * WorldPosition;
    vec4 prevScreenPos = ViewProjPrev * WorldPosition;
    gl_Position = screenPos;
    WorldPosition.w = screenPos.z; // Store linear depth
    TexCoord = vertexTexCoord;

    screenPos.xy /= screenPos.z;
    prevScreenPos.xy /= prevScreenPos.z;
    screenPos.xy = clamp(screenPos.xy, -1, 1);
    prevScreenPos.xy = clamp(prevScreenPos.xy, -1, 1);
    Velocity = vec3(screenPos.xy - prevScreenPos.xy, 0);
}