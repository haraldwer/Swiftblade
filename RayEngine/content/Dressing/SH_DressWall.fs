#version 330

// Uniforms
uniform vec3 cameraPosition;

// In
in vec3 worldPosition;
in vec3 worldNormal;
in vec3 objectPosition;
in vec4 screenPosition;

// Out
layout (location = 0) out vec3 OutPosition;
layout (location = 1) out vec3 OutNormal;
layout (location = 2) out vec4 OutColor;

void main()
{
    OutPosition = worldPosition;
    OutNormal = worldNormal;
    OutColor = vec4(0.0, 0.0, 1.0, 1.0);
}