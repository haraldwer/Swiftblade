#include "Shaders/Uniforms/SH_FrameUniforms.si"
#include "Shaders/Uniforms/SH_SceneOut.si"
#include "Shaders/Utility/SH_DeferredID.si"
#include "Shaders/Utility/SH_Utility.si"
#include "Shaders/Particles/SH_Particle_Common.si"

// In
in vec4 WorldPosition;
in vec3 WorldNormal;
in vec3 Velocity;

in vec2 TexCoord;
in vec2 SimCoord;

void main()
{
    OutPosition = WorldPosToTex(WorldPosition);
    OutNormal.xyz = WorldNormal;
    OutNormal.a = DeferredIDToTexID(DeferredID);
    OutData.xy = Velocity.xy;
    OutData.zw = vec2(0.0f, 1.0f);
}