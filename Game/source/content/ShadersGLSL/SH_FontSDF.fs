
// Input vertex attributes (from vertex shader)
in vec2 TexCoord;
in vec4 Color;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
layout (location = 0) out vec4 finalColor;

// NOTE: Add your custom variables here

void main()
{
    // Texel color fetching from texture sampler
    // NOTE: Calculate alpha using signed distance field (SDF)
    float distanceFromOutline = texture(texture0, TexCoord).a - 0.5f;
    float distanceChangePerFragment = length(vec2(dFdx(distanceFromOutline), dFdy(distanceFromOutline)));
    float alpha = smoothstep(-distanceChangePerFragment, distanceChangePerFragment, distanceFromOutline);
    
    // Calculate final fragment color
    finalColor = vec4(Color.rgb, Color.a * alpha);
}