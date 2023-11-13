#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;
in vec4 fragPos;
in vec4 fragNormal;

// Input uniform values
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = colDiffuse*fragColor;

    // Convert texel color to grayscale using NTSC conversion weights
    float gray = dot(texelColor.rgb, vec3(0.299, 0.587, 0.114));

    // Calculate final fragment color
    finalColor = vec4(gray, gray, gray, texelColor.a);
}