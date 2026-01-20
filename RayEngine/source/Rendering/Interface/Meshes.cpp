#include "Meshes.h"

int Rendering::RHI::GetLocation(DefaultAttributeLocation InLocation)
{
    return static_cast<int>(InLocation);
}

void Rendering::RHI::DrawQuad()
{
    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;

    float vertices[] = {
        // Positions         Texcoords
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f,
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f,
    };

    // Gen VAO to contain VBO
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    // Gen and fill vertex buffer (VBO)
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    // Bind vertex attributes (position, texcoords)
    glEnableVertexAttribArray(GetLocation(DefaultAttributeLocation::POSITION));
    glVertexAttribPointer(GetLocation(DefaultAttributeLocation::POSITION), 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)0); // Positions
    glEnableVertexAttribArray(GetLocation(DefaultAttributeLocation::TEXCOORD));
    glVertexAttribPointer(GetLocation(DefaultAttributeLocation::TEXCOORD), 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)(3*sizeof(float))); // Texcoords

    // Draw quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    // Delete buffers (VBO and VAO)
    glDeleteBuffers(1, &quadVBO);
    glDeleteVertexArrays(1, &quadVAO);
}


void Rendering::RHI::BindVertexArray(uint32 InVAO)
{
    glBindVertexArray(InVAO);
}

void Rendering::RHI::UnbindVertexArray()
{
    glBindVertexArray(0);
}

void Rendering::RHI::EnableVertexMatrixAttributes()
{
    // Set instance matrix data location
    const int matLoc = shader.locs[SHADER_LOC_MATRIX_MODEL];
    if (matLoc != -1)
    {
        for (unsigned int i = 0; i < 4; i++)
        {
            glEnableVertexAttribArray(matLoc + i);
            size_t offsetNative = static_cast<int>(i * sizeof(Vec4F));
            glVertexAttribPointer(matLoc + i, 4, GL_FLOAT, false, sizeof(Mat4F), (void*)offsetNative);
            glVertexAttribDivisor(matLoc + i, 1);
        }
    } 
}

uint32 Rendering::RHI::LoadMatrixVertexBuffer(const Vector<Mat4F> &InMatrices)
{
    // TODO: Do not recreate this every frame!
    unsigned int id = 0;
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    const int bufferSize = static_cast<int>(InMatrices.size() * sizeof(Mat4F));
    glBufferData(GL_ARRAY_BUFFER, bufferSize, InMatrices.data(), GL_STATIC_DRAW);
    return id;
}

uint32 Rendering::RHI::LoadVertexBuffer(const void *InBuffer, int InSize, bool InDynamic)
{
    unsigned int id = 0;
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, InSize, InBuffer, InDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    return id;
}

void Rendering::RHI::DisableVertexBuffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Rendering::RHI::DisableVertexBufferElement()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Rendering::RHI::UnloadVertexBuffer(const uint32 InVBO)
{
    glDeleteBuffers(1, &InVBO);
}

void Rendering::RHI::DrawElementsInstanced(const int InTriangleCount, const int InNumInstances)
{
    glDrawElementsInstanced(GL_TRIANGLES, InTriangleCount * 3, GL_UNSIGNED_SHORT, nullptr, InNumInstances);
}

void Rendering::RHI::DrawInstanced(const int InVertexCount, const int InDrawCount)
{
    glDrawArraysInstanced(GL_TRIANGLES, 0, InVertexCount, InDrawCount);
}
