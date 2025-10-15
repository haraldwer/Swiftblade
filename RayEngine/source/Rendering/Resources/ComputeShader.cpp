#include "ComputeShader.h"

#ifdef GRAPHICS_API_OPENGL_43

#include "external/glad.h"

bool Rendering::ComputeShaderResourceBase::Load()
{
    // Load shader source
    String code = Utility::ReadFile(id.Str());
    CHECK_RETURN_LOG(code.empty(), "Failed to load compute shader file: " + id.Str(), false)
    
    // Compile shader
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    auto cstr = code.c_str();
    glShaderSource(shader, 1, &cstr, nullptr);
    glCompileShader(shader);

    // Check for errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        constexpr int bufSize = 512;
        char log[bufSize];
        glGetShaderInfoLog(shader, bufSize, nullptr, log);
        LOG("Compute Shader Compilation Failed:" + String(log));
    }

    // Create program
    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);
    glDeleteShader(shader);

    return true;
}

void Rendering::ComputeShaderResourceBase::Dispatch(void *InPtr, int InSize)
{
    //glUseProgram(program);

    //// --- Create and load data ---
    //std::vector<float> input(1024);
    //for (int i = 0; i < 1024; ++i) input[i] = float(i);

    //GLuint inputSSBO, outputSSBO;
    //glGenBuffers(1, &inputSSBO);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, inputSSBO);
    //glBufferData(GL_SHADER_STORAGE_BUFFER, input.size() * sizeof(float), input.data(), GL_STATIC_DRAW);
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputSSBO); // binding = 0

    //glGenBuffers(1, &outputSSBO);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputSSBO);
    //glBufferData(GL_SHADER_STORAGE_BUFFER, input.size() * sizeof(float), nullptr, GL_STATIC_READ);
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputSSBO); // binding = 1
    //
    //GLuint numGroups = (1024 + 255) / 256; // ceil(1024/256)
    //glDispatchCompute(numGroups, 1, 1);
}
void Rendering::ComputeShaderResourceBase::Collect(void *InPtr, int &InSize)
{
    //// Read the data!
    //glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputSSBO);
    //float* ptr = static_cast<float*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY));
    //Vector output(ptr, ptr + input.size());
    //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

    //// --- Print results ---
    //for (int i = 0; i < 10; ++i)
    //    std::cout << input[i] << " -> " << output[i] << std::endl;


    //glDeleteBuffers(1, &inputSSBO);
    //glDeleteBuffers(1, &outputSSBO);
}

#endif
