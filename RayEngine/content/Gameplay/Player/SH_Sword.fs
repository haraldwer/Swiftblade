#include "Shaders/SH_Default_fs.si"
#include "Shaders/Utility/SH_Noise.si"

void main()
{
    Default();

    OutColor.rgb = vec3(0.3);
    OutData = vec3(0);
    
    if (VertexPosition.y < 0.345f)
        return;
    
    vec3 pos = (WorldPosition.xyz - ObjectPosition) * 1.0f + ObjectPosition * 0.2f;
    float noise = Simplex((pos - vec3(0.0f, Time * 5.0f, 0.0f)) * 0.02f, WorldNormal.xyz).r;
    noise = (noise + 1.0f) / 2.0f;
    noise = mix(0.2, 1.0, noise);
    OutColor.a = 1.0f - noise;
}
