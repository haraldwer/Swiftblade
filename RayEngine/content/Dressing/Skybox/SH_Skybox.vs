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

    screenPos.xy /= max(screenPos.z, 1);
    prevScreenPos.xy /= max(prevScreenPos.z, 1);
    screenPos.xy = clamp(screenPos.xy, -0.9999, 0.9999);
    prevScreenPos.xy = clamp(prevScreenPos.xy, -0.9999, 0.9999);
    Velocity = vec3(screenPos.xy - prevScreenPos.xy, 0);
}