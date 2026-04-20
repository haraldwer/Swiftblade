#include "Shaders/SH_Default_fs.si"

void main()
{
    vec3 pos = VertexPosition;
    float dist = min(min(
        abs(pos.x),
        abs(pos.y)),
        abs(pos.z));
    if (dist < 0.8f)
        discard;
    
    Default();
}