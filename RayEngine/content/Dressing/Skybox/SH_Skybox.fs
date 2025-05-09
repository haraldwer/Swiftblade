#include "Shaders/SH_Default_fs.si"
#include "Shaders/Utility/SH_Noise.si"
#include "Shaders/Utility/SH_HSV.si"

void main()
{
    Default();
    
    float h = CalcPerlin(normalize(VertexPosition) + vec3(0.0, -1.0, 0.0) * Time * 0.05);
    float s = CalcPerlin(normalize(VertexPosition) + vec3(0.0, -1.0, 0.0) * (Time * 0.07 + 123));
    float v = CalcPerlin(normalize(VertexPosition) + vec3(0.0, -1.0, 0.0) * (Time * 0.09 + 1234));
    OutColor.rgb = hsv2rgb(vec3(h, s, v));
    OutData.z = v;
}