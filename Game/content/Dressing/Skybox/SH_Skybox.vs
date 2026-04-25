#include "Shaders/SH_Default_vs.si"

void main()
{
    Default();
    
    vec3 vertPos = normalize(vertexPosition) * (NearFar.y - NearFar.x);
    VertexPosition = vertPos;
    WorldPosition = vec4(CameraPosition - vertPos, 1.0f);
    vec4 screenPos = ViewProj * WorldPosition;
    vec4 prevScreenPos = ViewProjPrev * WorldPosition;
    gl_Position = screenPos;
    WorldPosition.w = screenPos.z; // Store linear depth
    TexCoord = vertexTexCoord;

    screenPos.xy /= max(screenPos.z, 1.0f);
    prevScreenPos.xy /= max(prevScreenPos.z, 1.0f);
    screenPos.xy = clamp(screenPos.xy, -0.9999f, 0.9999f);
    prevScreenPos.xy = clamp(prevScreenPos.xy, -0.9999f, 0.9999f);
    Velocity = vec3((screenPos.xy - prevScreenPos.xy) * 0.5f, 0.0f);
}