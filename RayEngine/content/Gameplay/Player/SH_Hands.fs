#include "Shaders/SH_Default_fs.si"
#include "Shaders/Utility/SH_Noise.si"

void main()
{
    Default();

    OutColor.rgb = vec3(0.15);
    OutData = vec3(0);
}
