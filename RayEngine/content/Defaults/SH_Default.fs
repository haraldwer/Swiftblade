#version 330

// Default uniforms
uniform vec3 cameraPosition;

in vec3 worldPosition;
in vec3 worldNormal;
in vec3 objectPosition; 

out vec4 finalColor;

void main()
{
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
        
    vec3 color = vec3(cameraFade);
    finalColor = vec4(color, 1.0f);
}