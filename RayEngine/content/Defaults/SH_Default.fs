#version 330

in vec3 fragPosition;
in vec3 fragNormal;

out vec4 finalColor;

// NOTE: Add here your custom variables

void main()
{
    vec3 color = fragNormal;

    // Calculate final fragment color
    finalColor = vec4(color, 1.0f);
}