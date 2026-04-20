#include "Shaders/Uniforms/SH_FrameUniforms.si"
#include "Shaders/Particles/SH_Particle_Common.si"

// In
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

// Out
out vec4 WorldPosition;
out vec3 WorldNormal;
out vec3 Velocity;

out vec2 TexCoord;
out vec2 SimCoord;

uniform sampler2D TexPosition;
uniform sampler2D TexVelocity;
uniform vec2 TexPositionTexel;
uniform mat4 SystemTransform;

// Send a lot of stuff directly to fs
// Using flat keyword

vec2 GetSimCoord()
{
    float x = (gl_InstanceID * TexPositionTexel.x);
    float y = floor(x) * TexPositionTexel.y;
    return vec2(mod(x, 1.0f), y);
}

void main()
{
    vec2 coord = GetSimCoord();
    
    // Use gl_InstanceID
    vec4 position = texture(TexPosition, coord);
    vec4 velocity = texture(TexVelocity, coord);
    
    WorldNormal = normalize(mat3(SystemTransform) * vertexNormal);
    WorldPosition = (SystemTransform * vec4(vertexPosition * 0.1 + position.xyz, 1.0f));
    vec4 screenPos = ViewProj * WorldPosition;
    vec4 prevScreenPos = ViewProjPrev * WorldPosition;
    gl_Position = screenPos;
    WorldPosition.w = screenPos.z; // Store linear depth
    TexCoord = vertexTexCoord;
    SimCoord = coord;

    screenPos.xy /= max(screenPos.z, 1.0f);
    prevScreenPos.xy /= max(prevScreenPos.z, 1.0f);
    screenPos.xy = clamp(screenPos.xy, -0.9999f, 0.9999f);
    prevScreenPos.xy = clamp(prevScreenPos.xy, -0.9999f, 0.9999f);
    Velocity = vec3((screenPos.xy - prevScreenPos.xy) * 0.5f, 0.0f);
}