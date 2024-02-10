#version 330

// Uniforms
uniform vec3 CameraPosition;

// In
in vec3 WorldPosition;
in vec3 WorldNormal;
in vec3 ObjectPosition;
in vec4 ScreenPosition;

// Out
layout (location = 0) out vec3 OutPosition;
layout (location = 1) out vec3 OutNormal;
layout (location = 2) out vec4 OutColor;

void main()
{
    float dist = min(min(
        abs(ObjectPosition.x),
        abs(ObjectPosition.y)),
        abs(ObjectPosition.z));
    if (dist < 0.8f)
        discard;

    OutPosition = WorldPosition;
    OutNormal = WorldNormal;
    OutColor = vec4(1.0, 0.0, 0.0, 1.0);
}