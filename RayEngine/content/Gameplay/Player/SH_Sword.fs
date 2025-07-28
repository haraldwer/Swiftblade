#include "Shaders/SH_Default_fs.si"
#include "Shaders/Utility/SH_Noise.si"

void main()
{
    Default();
    OutData = vec4(0);
}
