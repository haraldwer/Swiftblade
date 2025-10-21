#include "Shaders/Uniforms/SH_FrameUniforms.si"
#include "Shaders/Particles/SH_Particle_Common.si"

// In
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

// Out
out vec4 WorldPosition;
out vec3 WorldNormal;
out vec3 ObjectPosition;
out vec3 VertexPosition;
out vec3 Velocity;
out vec2 TexCoord;

uniform sampler2D TexPosition;
uniform sampler2D TexVelocity;
uniform vec2 TexPositionTexel;
uniform mat4 SystemTransform;

// Send a lot of stuff directly to fs
// Using flat keyword

void main()
{
    //float x = (0 * TexPositionTexel.x);
    //float y = floor(x) * TexPositionTexel.y;
    //vec2 coord = vec2(modf(x, 1.0f), y);
    
    // Use gl_InstanceID
    vec2 coord = vec2(0);
    vec4 position = texture(TexPosition, coord) + vec4(sin(Time));
    vec4 velocity = texture(TexVelocity, coord);
    
    
    WorldNormal = normalize(mat3(SystemTransform) * vertexNormal);
    VertexPosition = vertexPosition + position.xyz;
    ObjectPosition = (SystemTransform * vec4(position.xyz, 1.0f)).xyz;
    WorldPosition = (SystemTransform * vec4(vertexPosition, 1.0f));
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