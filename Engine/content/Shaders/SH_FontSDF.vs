
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;

out vec2 TexCoord;      // To-fragment attribute: texture coordinate
out vec4 Color;         // To-fragment attribute: color

uniform mat4 mvp;

void main()
{
    TexCoord = vertexTexCoord;
    Color = vertexColor;
    gl_Position = mvp*vec4(vertexPosition, 1.0f);
}