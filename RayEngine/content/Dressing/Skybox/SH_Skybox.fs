#version 330

#include "Shaders/SH_Default_vs.si"
#include "Shaders/Uniforms/SH_FrameUniforms.si"
#include "Shaders/Utility/SH_DeferredID.si"
#include "Shaders/Utility/SH_Noise.si"

void main()
{
    VertexPosition = normalize(VertexPosition);
    VertexPosition *= NearFar.y;
    
    Default();
    
    float n = CalcPerlin(VertexPosition + vec3(0.0, -1.0, 0.0) * Time * 0.05);
    OutColor.rgb = vec3(n);
}