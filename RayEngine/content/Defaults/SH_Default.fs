#version 330

// Default uniforms
uniform vec3 cameraPosition;

in vec3 worldPosition;
in vec3 worldNormal;

out vec4 finalColor;

void main()
{
    // Calculate final fragment color
    vec3 cameraDirection = normalize(cameraPosition - worldPosition);
    vec3 color = vec3(dot(cameraDirection, worldNormal));
    finalColor = vec4(color, 1.0f);
}