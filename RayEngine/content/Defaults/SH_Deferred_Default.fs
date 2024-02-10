#version 330

#include "Shaders/SH_Random.si"
#include "Shaders/SH_HSV.si"

// Uniforms
uniform sampler2D TexPosition;
uniform sampler2D TexNormal;
uniform sampler2D TexColor;
uniform vec3 CameraPosition;
uniform vec2 Resolution;

// In
in vec2 TexCoord;
in vec2 TexCoord2;

// Out
out vec4 FinalColor;

vec4 EdgeDetection(vec3 InWorldPosition, vec3 InWorldNormal, vec3 InOriginalColor, float InCameraDot, float InCameraDistance)
{
    vec2 pixelSize = vec2(1.0f / Resolution.x, 1.0f / Resolution.y);
    
    // Color collection
    vec3 majorColor = vec3(0.0f);
    float maxColorValue = -1.0f; 

    // Value collection
    float value = 0.0f;
    float totalValue = 0.0f;
    
    int samples = 32;
    float maxOffset = 5.0f;
    for (int i = 0; i < samples; i++)
    {
        // Calculate sample coordinate
        float sampleDistance = (float(i) / float(samples));
        vec2 rand = Random2(sampleDistance + Random(InWorldPosition)).xy * 2.0f - vec2(1.0f);
        float offsetScale = sampleDistance * maxOffset;
        vec2 offset = TexCoord + rand * pixelSize * offsetScale;
        vec2 sampleCoord = vec2(
            clamp(offset.x, 0.001f, 0.999f),
            clamp(offset.y, 0.001f, 0.999f));
        
        // Calculate weight
        float weight = 1.0f / (sampleDistance + 1.0f); 
        
        // Position
        float maxDistance = 1.0f;
        float minDistance = 0.01f;
        vec3 samplePosition = texture(TexPosition, sampleCoord).xyz; 
        vec3 diff = samplePosition - InWorldPosition;
        float distSqr = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
        float positionValue = clamp(distSqr - minDistance, 0.0f, maxDistance); 
        
        // Normal 
        vec3 sampleNormal = texture(TexNormal, sampleCoord).xyz;
        float normalDot = dot(sampleNormal, InWorldNormal);
        float normalValue = float(normalDot < 0.1f);

        // Color 
        vec3 sampleColor = texture(TexColor, sampleCoord).rgb;
        vec3 hsv = rgb2hsv(sampleColor);
        float colorValue = hsv.g; // saturation, luminance
        if (colorValue > maxColorValue)
        {
            maxColorValue = colorValue;
            hsv.b = mix(0.7f, hsv.b, hsv.g); // luminance relative to saturation
            majorColor = hsv2rgb(hsv); 
        }
        
        // Blend
        value += mix(normalValue, positionValue, InCameraDot) * weight;
        totalValue += weight;
    }
    value /= totalValue;
    
    // Perlin noise
    float perlin = PerlinNoise3D(InWorldPosition * 0.5f); 
    float perlinCutoff = (1.0f - perlin) * 0.1f;
    value = step(perlinCutoff, value);
    
    return vec4(majorColor, value); 
}

void main()
{
    // Sample textures
    vec3 worldPosition = texture(TexPosition, TexCoord).rgb;
    vec3 worldNormal = texture(TexNormal, TexCoord).xyz;
    vec4 color = texture(TexColor, TexCoord).rgba;
    
    // Calculate final fragment color
    vec3 cameraDifference = CameraPosition - worldPosition;
    float cameraDistance = length(cameraDifference);
    vec3 cameraDirection = cameraDifference / cameraDistance;
    float cameraDot = dot(cameraDirection, worldNormal);
    
    // Fade
    const float startFade = 20.0f;
    const float endFade = 50.0f;
    float cameraDistanceFade = 1.0f - clamp((cameraDistance - startFade) / (endFade - startFade), 0.0f, 1.0f);
    float cameraFade = mix(cameraDistanceFade, cameraDot * cameraDistanceFade, 0.5f);
    if (cameraFade < 0.01)
        discard;
    
    // Edge detection
    vec4 edge = EdgeDetection(worldPosition, worldNormal, FinalColor.rgb, cameraDot, cameraDistance);
    color.rgb = mix(color.rgb, edge.rgb, edge.a * color.a);
    
    FinalColor = vec4(color.rgb * vec3(cameraFade), 1.0f);
}