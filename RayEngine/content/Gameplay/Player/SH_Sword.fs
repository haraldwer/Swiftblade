#version 330

#include "Shaders/SH_Default_fs.si"
#include "Shaders/Utility/SH_Random.si"

void main()
{
    Default();

    OutColor.rgb = vec3(0.3);
    
    if (VertexPosition.y < 0.345f)
        return;
    
    vec3 pos = (OutPosition.xyz - ObjectPosition) * 1.0f + ObjectPosition * 0.2f;
    float noise = PerlinNoise3D(pos - vec3(0.0f, Time * 5.0f, 0.0f));
    noise = (noise + 1.0f) / 2.0f;
    OutDeferredData.g = noise;
}