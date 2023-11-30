#version 330

// Default uniforms
uniform vec3 cameraPosition;

in vec3 worldPosition;
in vec3 worldNormal;
in vec3 objectPosition; 

out vec4 finalColor;

void main()
{
    float dist = min(min(
        abs(objectPosition.x),
        abs(objectPosition.y)),
        abs(objectPosition.z));
    if (dist < 0.8f)
        discard; 

    vec3 color = vec3(1.0f, 0.0f, 0.0f);
    finalColor = vec4(vec3(color), 1.0f);
}