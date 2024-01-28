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
out vec3 worldPosition;
out vec3 worldNormal;
out vec3 objectPosition; 
out vec4 screenPosition;

void main()
{
    vec3 objectNormal = normalize(vec3(matNormal * vec4(vertexNormal, 1.0)));
    worldNormal = normalize(mat3(instanceTransform) * objectNormal);

    objectPosition = vertexPosition;
    worldPosition = vec3(instanceTransform * vec4(vertexPosition, 1.0));

    // Calculate final vertex position
    mat4 mvpi = mvp * instanceTransform;
    screenPosition = mvpi * vec4(vertexPosition, 1.0);

    // Push things outwards based on distance from screen center
    // Closer to center = more push 

    float distMul = 0.5;
    float distPow = 0.7; 
    float pushMul = 0.3; 

    float dist = distance(screenPosition.xy, vec2(0.0)) / screenPosition.z;
    float clamped = clamp(dist * distMul, 0.0, 1.0);
    float push = pow(clamped, distPow) * pushMul;
    float compensation = pow(1.5 * distMul, distPow) * pushMul;
    screenPosition.xy *= 1.0 - push + compensation;

    gl_Position = screenPosition;
}