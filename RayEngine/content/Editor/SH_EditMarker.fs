#include "Shaders/SH_Default_fs.si"

void main()
{
    vec3 pos = VertexPosition;//WorldPosition.xyz - ObjectPosition.xyz;
    float dist = min(min(
        abs(pos.x),
        abs(pos.y)),
        abs(pos.z));
    if (dist < 0.8f)
        discard;
    
    Default();
}