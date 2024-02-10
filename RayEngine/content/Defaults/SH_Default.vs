#version 330

// Uniforms
uniform mat4 mvp;
uniform mat4 matNormal;

// In
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in mat4 instanceTransform;

// Out
out vec3 WorldPosition;
out vec3 WorldNormal;
out vec3 ObjectPosition; 
out vec4 ScreenPosition;

void main()
{
    vec3 objectNormal = normalize(vec3(matNormal * vec4(vertexNormal, 1.0)));
    WorldNormal = normalize(mat3(instanceTransform) * objectNormal);

    ObjectPosition = vertexPosition;
    WorldPosition = vec3(instanceTransform * vec4(vertexPosition, 1.0));

    // Calculate final vertex position
    mat4 mvpi = mvp * instanceTransform;
    ScreenPosition = mvpi * vec4(vertexPosition, 1.0);

    // Push things outwards based on distance from screen center
    // Closer to center = more push 

    float distMul = 0.5;
    float distPow = 0.7; 
    float pushMul = 0.3; 

    float dist = distance(ScreenPosition.xy, vec2(0.0)) / ScreenPosition.z;
    float clamped = clamp(dist * distMul, 0.0, 1.0);
    float push = pow(clamped, distPow) * pushMul;
    float compensation = pow(1.5 * distMul, distPow) * pushMul;
    ScreenPosition.xy *= 1.0 - push + compensation;

    gl_Position = ScreenPosition;
}