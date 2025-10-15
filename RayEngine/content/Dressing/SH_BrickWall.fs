#include "Shaders/SH_Default_fs.si"
#include "Shaders/Utility/SH_Noise.si"

vec2 AxisStep(float InAxis, vec3 InPos, float InScale, float InOffset, float InPadding)
{
    InAxis += Random(InOffset) * 10.0f * InScale;
    float steps = round(InAxis * InScale);
    float step = pow(abs(fract(InAxis * InScale) - 0.5f) * 2.0f - InPadding, 0.2f);
    return vec2(step, steps);
}

void main()
{
    Default();

    vec3 pos = WorldPosition.xyz;
    vec3 normal = WorldNormal;

    vec2 heightBricks = AxisStep(pos.y, pos, 1.0f, 0.0f, 0.07f);
    vec2 xBricks = AxisStep(pos.x, pos, 0.5f, heightBricks.y, 0.05f);
    vec2 zBricks = AxisStep(pos.z, pos, 0.5f, heightBricks.y, 0.05f);

    float brick =
        xBricks.x *
        zBricks.x *
        heightBricks.x;

    float brickFade = Random(vec3(xBricks.y, zBricks.y, heightBricks.y));
    float edgeNoise = abs(Simplex(pos * 0.05f, normal).r);
    brick -= edgeNoise * 0.25f * (1.0f - brick);
    brick = clamp(brick, 0.0f, 1.0f);

    float bigNoise = Simplex(pos * 0.005f, normal).r;
    bigNoise += abs(normal.y) * 0.5f;
    brick *= step(0.2f, bigNoise * brick);
    
    OutData.z = 1 - brick;
    
    float n = -(CalcSimplex(pos * 3.0f) + 1.0f) / 2.0f; 
    OutData.z += n * 0.2f * clamp((1.0f - brick * 2.0f), 0.0f, 1.0f);
}