#version 330

#include "Shaders/SH_Default_fs.si"

void main()
{
    float dist = min(min(
        abs(ObjectPosition.x),
        abs(ObjectPosition.y)),
        abs(ObjectPosition.z));
    if (dist < 0.8f)
        discard;
    
    Default();
    
    OutColor = vec4(1.0, 0.0, 0.0, 1.0);
}