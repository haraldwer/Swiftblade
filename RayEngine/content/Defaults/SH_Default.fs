#version 330

#include "Shaders/SH_FrameUniforms.si"
#include "Shaders/SH_ObjectUniforms.si"

// In
in vec3 WorldPosition;
in vec3 WorldNormal;
in vec3 ObjectPosition; 
in vec4 ScreenPosition; 

// Out
layout (location = 0) out vec3 OutPosition;
layout (location = 1) out vec3 OutNormal;
layout (location = 2) out vec4 OutColor;
layout (location = 3) out uint OutID;

void main()
{
    OutPosition = WorldPosition;
    OutNormal = WorldNormal; 
    OutColor = vec4(1.0);
    OutID = (float)DeferredID;
    
    // TODO: Motion vectors
}