#include "Shaders/SH_Default_fs.si"
#include "Shaders/Utility/SH_Noise.si"

vec2 AxisStep(float InAxis, vec3 InPos, float InScale, float InOffset, float InPadding)
{
    InAxis += Random(InOffset) * 10.0 * InScale;
    float steps = round(InAxis * InScale);
    float step = pow(abs(fract(InAxis * InScale) - 0.5) * 2.0 - InPadding, 0.2);
    return vec2(step, steps);
}

void main()
{
    Default();

    vec3 pos = WorldPosition.xyz;
    vec3 normal = WorldNormal;

    vec2 heightBricks = AxisStep(pos.y, pos, 1.0, 0.0, 0.07);
    vec2 xBricks = AxisStep(pos.x, pos, 0.5, heightBricks.y, 0.05);
    vec2 zBricks = AxisStep(pos.z, pos, 0.5, heightBricks.y, 0.05);

    float brick =
        xBricks.x *
        zBricks.x *
        heightBricks.x;

    float brickFade = Random(vec3(xBricks.y, zBricks.y, heightBricks.y));
    float edgeNoise = abs(Simplex(pos * 0.05, normal).r);
    brick -= edgeNoise * 0.25 * (1.0 - brick);
    brick = clamp(brick, 0.0, 1.0);

    float bigNoise = Simplex(pos * 0.005, normal).r;
    bigNoise += abs(normal.y) * 0.5;
    brick *= step(0.2, bigNoise * brick);
    
    OutData.z = 1 - brick;
    
    float n = -(CalcSimplex(pos * 3) + 1) / 2; 
    OutData.z += n * 0.2 * clamp((1 - brick * 2), 0, 1);
}