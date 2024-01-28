#version 330

// Uniforms
uniform sampler2D TexPosition;
uniform sampler2D TexNormal;
uniform sampler2D TexColor;
uniform vec3 cameraPosition;

// In
in vec2 texCoord;
in vec2 texCoord2;

// Out
out vec4 finalColor;

void main()
{
    vec3 worldPosition = texture(TexPosition, texCoord).rgb;
    vec3 worldNormal = texture(TexNormal, texCoord).rgb;
    vec4 color = texture(TexColor, texCoord).rgba;
    if (color.a < 0.01)
        discard; 

    // Calculate final fragment color
    vec3 cameraDifference = cameraPosition - worldPosition;
    float cameraDistance = length(cameraDifference);
    vec3 cameraDirection = cameraDifference / cameraDistance;
    float cameraDot = dot(cameraDirection, worldNormal);

    const float startFade = 1.0f;
    const float fadeDist = 50.0f;
    float cameraDistanceFade = 1.0f - clamp((cameraDistance - startFade) / fadeDist, 0.0f, 1.0f);
    float cameraFade = mix(cameraDistanceFade, cameraDot * cameraDistanceFade, 0.5f);
    if (cameraFade < 0.01)
        discard;
    
    finalColor = vec4(color.rgb * vec3(cameraFade), 1.0f);
}