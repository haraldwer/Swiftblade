#version 330

// Default uniforms
uniform mat4 mvp;
uniform mat4 matNormal;

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in mat4 instanceTransform;

out vec3 worldPosition;
out vec3 worldNormal;

void main()
{

    vec3 objectNormal = normalize(vec3(matNormal * vec4(vertexNormal, 1.0)));
    worldNormal = normalize(mat3(instanceTransform) * objectNormal);

    worldPosition = vec3(instanceTransform * vec4(vertexPosition, 1.0));


    // Calculate final vertex position
    mat4 mvpi = mvp*instanceTransform;
    gl_Position = mvpi*vec4(vertexPosition, 1.0);
}