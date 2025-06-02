#pragma once

#include <external/glad.h>

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

namespace RaylibRenderUtility
{
    inline Array<QuatF, 6> GetCubemapRotations()
    {
        return {
            QuatF::FromEuler({ 0, PI/2, PI/2 }), //Right
            QuatF::FromEuler({ 0, -PI/2, -PI/2 }), //Left
            QuatF::FromEuler({ PI/2, 0, PI }), //Up
            QuatF::FromEuler({ -PI/2, 0, PI }), //Down
            QuatF::FromEuler({ 0, 0, PI }), //Forward
            QuatF::FromEuler({ PI, 0, 0 }), //Backward
        };
    }

    inline void SetBlendMode(const int InMode)
    {
        if (InMode < 0)
        {
            rlDisableColorBlend();
        }
        else
        {
            rlEnableColorBlend();
            switch (InMode)
            {
            case RL_BLEND_ALPHA: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glBlendEquation(GL_FUNC_ADD); break;
            case RL_BLEND_ADDITIVE: glBlendFunc(GL_SRC_ALPHA, GL_ONE); glBlendEquation(GL_FUNC_ADD); break;
            case RL_BLEND_MULTIPLIED: glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA); glBlendEquation(GL_FUNC_ADD); break;
            case RL_BLEND_ADD_COLORS: glBlendFunc(GL_ONE, GL_ONE); glBlendEquation(GL_FUNC_ADD); break;
            case RL_BLEND_SUBTRACT_COLORS: glBlendFunc(GL_ONE, GL_ONE); glBlendEquation(GL_FUNC_SUBTRACT); break;
            case RL_BLEND_ALPHA_PREMULTIPLY: glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); glBlendEquation(GL_FUNC_ADD); break;
            }
        }
    }
    
    inline void DrawInstances(const Mesh& InMesh, const Shader& InShader, const Vector<Mat4F>& InMatrices)
    {
        int instances = static_cast<int>(InMatrices.size());
        
        // Send required data to shader (matrices, values)
        //-----------------------------------------------------
        
        // Get a copy of current matrices to work with,
        // just in case stereo render is required, and we need to modify them
        // NOTE: At this point the modelview matrix just contains the view matrix (camera)
        // That's because BeginMode3D() sets it and there is no model-drawing function
        // that modifies it, all use rlPushMatrix() and rlPopMatrix()
        Matrix matModel = MatrixIdentity();
        Matrix matView = rlGetMatrixModelview();
        Matrix matModelView = MatrixIdentity();
        Matrix matProjection = rlGetMatrixProjection();
        
        // Upload view and projection matrices (if locations available)
        if (InShader.locs[SHADER_LOC_MATRIX_VIEW] != -1)
            rlSetUniformMatrix(InShader.locs[SHADER_LOC_MATRIX_VIEW], matView);
        if (InShader.locs[SHADER_LOC_MATRIX_PROJECTION] != -1)
            rlSetUniformMatrix(InShader.locs[SHADER_LOC_MATRIX_PROJECTION], matProjection);
            
        // Enable mesh VAO to attach new buffer
        rlEnableVertexArray(InMesh.vaoId);

        // This could alternatively use a static VBO and either glMapBuffer() or glBufferSubData().
        // It isn't clear which would be reliably faster in all cases and on all platforms,
        // anecdotally glMapBuffer() seems very slow (syncs) while glBufferSubData() seems
        // no faster, since we're transferring all the transform matrices anyway
        unsigned int instancesVboId = 0;
        int bufferSize = instances * static_cast<int>(sizeof(Mat4F));
        instancesVboId = rlLoadVertexBuffer(InMatrices.data(), bufferSize, false);

        // Instances transformation matrices are send to shader attribute location: SHADER_LOC_MATRIX_MODEL
        for (unsigned int i = 0; i < 4; i++)
        {
            rlEnableVertexAttribute(InShader.locs[SHADER_LOC_MATRIX_MODEL] + i);
            rlSetVertexAttribute(InShader.locs[SHADER_LOC_MATRIX_MODEL] + i, 4, RL_FLOAT, false, sizeof(Mat4F), static_cast<int>(i * sizeof(Vec4F)));
            rlSetVertexAttributeDivisor(InShader.locs[SHADER_LOC_MATRIX_MODEL] + i, 1);
        }

        rlDisableVertexBuffer();
        rlDisableVertexArray();

        // Accumulate internal matrix transform (push/pop) and view matrix
        // NOTE: In this case, model instance transformation must be computed in the shader
        matModelView = MatrixMultiply(rlGetMatrixTransform(), matView);

        // Upload model normal matrix (if locations available)
        if (InShader.locs[SHADER_LOC_MATRIX_NORMAL] != -1)
            rlSetUniformMatrix(InShader.locs[SHADER_LOC_MATRIX_NORMAL], MatrixTranspose(MatrixInvert(matModel)));
        //-----------------------------------------------------

        // Try binding vertex array objects (VAO)
        // or use VBOs if not possible
        if (!rlEnableVertexArray(InMesh.vaoId))
        {
            // Bind mesh VBO data: vertex position (shader-location = 0)
            rlEnableVertexBuffer(InMesh.vboId[0]);
            rlSetVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_POSITION], 3, RL_FLOAT, false, 0, 0);
            rlEnableVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_POSITION]);

            // Bind mesh VBO data: vertex texcoords (shader-location = 1)
            rlEnableVertexBuffer(InMesh.vboId[1]);
            rlSetVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_TEXCOORD01], 2, RL_FLOAT, false, 0, 0);
            rlEnableVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_TEXCOORD01]);

            if (InShader.locs[SHADER_LOC_VERTEX_NORMAL] != -1)
            {
                // Bind mesh VBO data: vertex normals (shader-location = 2)
                rlEnableVertexBuffer(InMesh.vboId[2]);
                rlSetVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_NORMAL], 3, RL_FLOAT, false, 0, 0);
                rlEnableVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_NORMAL]);
            }

            // Bind mesh VBO data: vertex colors (shader-location = 3, if available)
            if (InShader.locs[SHADER_LOC_VERTEX_COLOR] != -1)
            {
                if (InMesh.vboId[3] != 0)
                {
                    rlEnableVertexBuffer(InMesh.vboId[3]);
                    rlSetVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_COLOR], 4, RL_UNSIGNED_BYTE, true, 0, 0);
                    rlEnableVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_COLOR]);
                }
                else
                {
                    // Set default value for unused attribute
                    // NOTE: Required when using default shader and no VAO support
                    float value[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                    rlSetVertexAttributeDefault(InShader.locs[SHADER_LOC_VERTEX_COLOR], value, SHADER_ATTRIB_VEC4, 4);
                    rlDisableVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_COLOR]);
                }
            }

            // Bind mesh VBO data: vertex tangents (shader-location = 4, if available)
            if (InShader.locs[SHADER_LOC_VERTEX_TANGENT] != -1)
            {
                rlEnableVertexBuffer(InMesh.vboId[4]);
                rlSetVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_TANGENT], 4, RL_FLOAT, false, 0, 0);
                rlEnableVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_TANGENT]);
            }

            // Bind mesh VBO data: vertex texcoords2 (shader-location = 5, if available)
            if (InShader.locs[SHADER_LOC_VERTEX_TEXCOORD02] != -1)
            {
                rlEnableVertexBuffer(InMesh.vboId[5]);
                rlSetVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_TEXCOORD02], 2, RL_FLOAT, false, 0, 0);
                rlEnableVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_TEXCOORD02]);
            }

            if (InMesh.indices != nullptr)
                rlEnableVertexBufferElement(InMesh.vboId[6]);
        }

        // WARNING: Disable vertex attribute color input if mesh can not provide that data (despite location being enabled in shader)
        if (InMesh.vboId[3] == 0)
            rlDisableVertexAttribute(InShader.locs[SHADER_LOC_VERTEX_COLOR]);

        int eyeCount = rlIsStereoRenderEnabled() ? 2 : 1;
        for (int eye = 0; eye < eyeCount; eye++)
        {
            // Calculate model-view-projection matrix (MVP)
            Matrix matModelViewProjection = MatrixIdentity();
            if (eyeCount == 1) matModelViewProjection = MatrixMultiply(matModelView, matProjection);
            else
            {
                // Setup current eye viewport (half screen width)
                rlViewport(eye*rlGetFramebufferWidth()/2, 0, rlGetFramebufferWidth()/2, rlGetFramebufferHeight());
                matModelViewProjection = MatrixMultiply(MatrixMultiply(matModelView, rlGetMatrixViewOffsetStereo(eye)), rlGetMatrixProjectionStereo(eye));
            }

            // Send combined model-view-projection matrix to shader
            rlSetUniformMatrix(InShader.locs[SHADER_LOC_MATRIX_MVP], matModelViewProjection);

            // Draw mesh instanced
            if (InMesh.indices != nullptr)
                rlDrawVertexArrayElementsInstanced(0, InMesh.triangleCount * 3, nullptr, instances);
            else
                rlDrawVertexArrayInstanced(0, InMesh.vertexCount, instances);
        }

        // Disable all possible vertex array objects (or VBOs)
        rlDisableVertexArray();
        rlDisableVertexBuffer();
        rlDisableVertexBufferElement();

        // Remove instance transforms buffer
        rlUnloadVertexBuffer(instancesVboId);
    }
}