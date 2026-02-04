#include "Shaders/Uniforms/SH_FrameUniforms.si"
#include "Shaders/Utility/SH_Utility.si"

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 VertexTexCoord;

out vec2 TexCoord;
out vec2 RectCoord;

void main() 
{
    RectCoord = VertexTexCoord;
    TexCoord = GetRectUV(VertexTexCoord, Rect);
    gl_Position = vec4(VertexPosition, 1.0f);
    gl_Position.w = 1.0f;
}