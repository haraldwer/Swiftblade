#include "Shaders/SH_Default_fs.si"
#include "Shaders/Utility/SH_Noise.si"

void main()
{
    Default();
    OutData.xy = vec2(0.0f);
}
