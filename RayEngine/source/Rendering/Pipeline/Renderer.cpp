#include "Renderer.h"

#include "Context/Context.h"
#include "Core/Utility/RayUtility.h"
#include "Lights/Lights.h"
#include "Lumin/Lumin.h"
#include "RayRenderUtility.h"
#include "Scene/Scene.h"
#include "State/Command.h"
#include "State/State.h"
#include "Viewport/Viewport.h"
#include "Resources/BakedTexture.h"

void Rendering::Renderer::SetValue(ShaderResource& InShader, const String& InName, const void* InValue, const int InType, const int InCount)
{
    const int loc = InShader.GetLocation(InName);
    CHECK_RETURN(loc < 0);
    PROFILE_GL_GPU("Set uniform")
    rlSetUniform(loc, InValue, InType, InCount);
}

void Rendering::Renderer::SetValue(ShaderResource& InShader, const String& InName, const Mat4F& InValue)
{
    const int loc = InShader.GetLocation(InName);
    CHECK_RETURN(loc < 0);
    PROFILE_GL_GPU("Set matrix uniform")
    const Matrix m = Utility::Ray::ConvertMat(InValue);
    rlSetUniformMatrix(loc, m);
}

void Rendering::Renderer::SetValue(const ShaderResource& InShader, const ShaderResource::DefaultLoc& InLoc, const void* InValue, const int InType, const int InCount)
{
    const int loc = InShader.GetLocation(InLoc);
    CHECK_RETURN(loc < 0);
    PROFILE_GL_GPU("Set uniform")
    rlSetUniform(loc, InValue, InType, InCount);
}

void Rendering::Renderer::SetValue(const ShaderResource& InShader, const ShaderResource::DefaultLoc& InLoc, const Mat4F& InValue)
{
    const int loc = InShader.GetLocation(InLoc);
    CHECK_RETURN(loc < 0);
    const Matrix m = Utility::Ray::ConvertMat(InValue);
    rlSetUniformMatrix(loc, m);
}

void Rendering::Renderer::SetFrameShaderValues(const RenderArgs& InArgs, ShaderResource& InShader, const RenderTarget& InSceneTarget)
{
    PROFILE_GL();

    const auto& viewport = *InArgs.viewportPtr;
    auto& context = *InArgs.contextPtr;
    const auto ptr = InShader.Get();
    CHECK_RETURN(!ptr);

    // Time
    const float time = static_cast<float>(context.timer.Ellapsed()); 
    SetValue(InShader, ShaderResource::DefaultLoc::TIME, &time, SHADER_UNIFORM_FLOAT);
    const float delta = static_cast<float>(viewport.delta); 
    SetValue(InShader, ShaderResource::DefaultLoc::DELTA, &delta, SHADER_UNIFORM_FLOAT);
    const Vec2F res = viewport.GetResolution().To<float>();
    SetValue(InShader, ShaderResource::DefaultLoc::RESOLUTION, &res, SHADER_UNIFORM_VEC2);
}

void Rendering::Renderer::SetPerspectiveShaderValues(const RenderArgs& InArgs, const Perspective& InPerspective, ShaderResource& InShader)
{
    PROFILE_GL();
    
    auto& viewport = *InArgs.viewportPtr;
    auto ptr = InShader.Get();
    CHECK_RETURN(!ptr);

    const Vec2F res = viewport.GetResolution().To<float>();
    Vec4F refRect = {
        InPerspective.referenceRect.x,
        InPerspective.referenceRect.y,
        InPerspective.referenceRect.z > 0.0f ? InPerspective.referenceRect.z : 1.0f,
        InPerspective.referenceRect.w > 0.0f ? InPerspective.referenceRect.w : 1.0f
    };
    Vec4F rect = {
        InPerspective.targetRect.x,
        InPerspective.targetRect.y,
        InPerspective.targetRect.z > 0.0f ? InPerspective.targetRect.z : 1.0f,
        InPerspective.targetRect.w > 0.0f ? InPerspective.targetRect.w : 1.0f
    };
    
    // Matrices
    Vec2F size = Vec2F(rect.z, rect.w) * res;
    size = { floorf(size.x), floorf(size.y) };
    Mat4F view = InPerspective.camera.GetViewMatrix();
    Mat4F proj = InPerspective.camera.GetProjectionMatrix(size);
    viewport.viewProj = Mat4F::GetInverse(view) * proj;
    
    SetValue(InShader, ShaderResource::DefaultLoc::VIEW_PROJ, Mat4F::Transpose(viewport.viewProj));
    SetValue(InShader, ShaderResource::DefaultLoc::VIEW_PROJ_PREV, Mat4F::Transpose(viewport.viewProjPrev));
    SetValue(InShader, ShaderResource::DefaultLoc::VIEW_PROJ_INV, Mat4F::Transpose(Mat4F::GetInverse(viewport.viewProj)));
    
    SetValue(InShader, ShaderResource::DefaultLoc::RECT, &rect, SHADER_UNIFORM_VEC4);
    SetValue(InShader, ShaderResource::DefaultLoc::REF_RECT, &refRect, SHADER_UNIFORM_VEC4);

    // Camera and view
    SetValue(InShader, ShaderResource::DefaultLoc::CAMERA_POSITION, &InPerspective.camera.position, SHADER_UNIFORM_VEC3);
    Vec2F nearFar = { InPerspective.camera.near, InPerspective.camera.far };
    SetValue(InShader, ShaderResource::DefaultLoc::NEAR_FAR, &nearFar, SHADER_UNIFORM_VEC2);
}

void Rendering::Renderer::SetCustomShaderValues(ShaderResource& InShader)
{
    // TODO
}

void Rendering::Renderer::BindNoiseTextures(const RenderArgs& InArgs, ShaderResource& InShader, int& InOutSlot)
{
    PROFILE_GL();
    CHECK_ASSERT(!InArgs.contextPtr, "Invalid context");
    
    auto& config = InArgs.contextPtr->config;
    for (auto& entry : config.NoiseTextures.Get())
    {
        const int loc = InShader.GetLocation(entry.first);
        CHECK_CONTINUE(loc < 0);
        const auto noiseRes = entry.second.Get();
        CHECK_CONTINUE(!noiseRes);
        const auto texRes = noiseRes->Get().Get();
        CHECK_CONTINUE(!texRes);
        const auto tex = texRes->Get();
        CHECK_CONTINUE(!tex);
        
        InOutSlot++;
        TextureCommand cmd;
        cmd.shaderLoc = loc;
        cmd.id = tex->id;
        cmd.filter = RL_TEXTURE_FILTER_LINEAR;
        cmd.wrap = RL_TEXTURE_WRAP_REPEAT;
        rlState::current.Set(cmd, InOutSlot);
    }
}


void Rendering::Renderer::DrawQuad()
{
    rlState::current.ResetMesh();
    PROFILE_GL_GPU("Draw quad");
    rlLoadDrawQuad();
}


int Rendering::Renderer::DrawInstances(const Mesh& InMesh, int InCount)
{
    PROFILE_GL_GPU("Draw instances");
    if (InMesh.indices != nullptr)
        rlDrawVertexArrayElementsInstanced(0, InMesh.triangleCount * 3, nullptr, InCount);
    else
        rlDrawVertexArrayInstanced(0, InMesh.vertexCount, InCount);
    return InCount;
}

void Rendering::Renderer::DrawFullscreen(const RenderArgs& InArgs, const RenderTarget& InTarget, const ResShader& InShader, const Vector<RenderTarget*>& InBuffers, int InBlend, bool InClear)
{
    PROFILE_GL();
    
    ShaderResource* shaderResource = InShader.Get();
    CHECK_RETURN_LOG(!shaderResource, "Failed to find shader resource");
    const Shader* shader = shaderResource->Get();
    CHECK_RETURN_LOG(!shader, "Failed to get shader");

    FrameCommand frameCmd;
    frameCmd.fboID = InTarget.GetFBO();
    frameCmd.size = InTarget.Size();
    frameCmd.clearTarget = InClear;
    rlState::current.Set(frameCmd);

    ShaderCommand shaderCmd;
    shaderCmd.locs = shader->locs;
    shaderCmd.id = shader->id;
    shaderCmd.blendMode = InBlend;
    rlState::current.Set(shaderCmd);
    
    SetFrameShaderValues(InArgs, *shaderResource, InTarget);
    
    int texSlot = 0;
    for (auto& b : InBuffers)
        if (b) b->Bind(*shaderResource, texSlot);
    BindNoiseTextures(InArgs, *shaderResource, texSlot);
    
    for (auto& persp : InArgs.perspectives)
    {
        PerspectiveCommand perspCmd;
        perspCmd.rect = persp.targetRect;
        rlState::current.Set(perspCmd);
        SetPerspectiveShaderValues(InArgs, persp, *shaderResource);
        DrawQuad();
    }
}

int Rendering::Renderer::DrawDebug(const RenderArgs& InArgs)
{
    PROFILE_GL();
    
    auto& scene = *InArgs.scenePtr;
    rlState::current.Reset();

    BeginTextureMode(InArgs.viewportPtr->GetVirtualTarget());

    for (auto& persp : InArgs.perspectives)
    {
        BeginMode3D(Utility::Ray::ConvertCamera(persp.camera));

        const Vec2F res = InArgs.viewportPtr->GetResolution().To<float>();
        float w = persp.targetRect.z > 0 ?
            static_cast<float>(persp.targetRect.z) : res.x;
        float h = persp.targetRect.w > 0 ?
            static_cast<float>(persp.targetRect.w) : res.y;
        Vec4I rect = Vec4F({
            static_cast<float>(persp.targetRect.x),
            static_cast<float>(persp.targetRect.y),
            w, h }).To<int>();
        
        rlViewport(rect.x, rect.y, rect.z, rect.w);
        rlEnableSmoothLines();

        for (auto& shape : scene.debugShapes.GetCulled(InArgs.cullPoints))
        {
            switch (shape.type)
            {
            case DebugShape::Type::SPHERE:
                DrawSphereWires(
                    Utility::Ray::ConvertVec(shape.pos),
                    shape.data.x,
                    static_cast<int>(shape.data.y),
                    static_cast<int>(shape.data.z),
                    { shape.col.r, shape.col.g, shape.col.b, shape.col.a });
                break;
            case DebugShape::Type::BOX:
                DrawCubeWiresV(
                    Utility::Ray::ConvertVec(shape.pos),
                    Utility::Ray::ConvertVec(shape.data),
                    { shape.col.r, shape.col.g, shape.col.b, shape.col.a });
                break;
            case DebugShape::Type::CAPSULE:
                const Vec3F dir = Mat4F(shape.rot).Right() * shape.data.y;
                const auto start = Utility::Ray::ConvertVec(shape.pos + dir);
                const auto end = Utility::Ray::ConvertVec(shape.pos - dir);
                DrawCapsuleWires(
                    start,
                    end,
                    shape.data.x,
                    static_cast<int>(shape.data.z),
                    static_cast<int>(shape.data.z) / 2,
                    { shape.col.r, shape.col.g, shape.col.b, shape.col.a });
                break;
            }
        }

        for (auto& line : scene.debugLines.GetCulled(InArgs.cullPoints))
            DrawLine3D(
                Utility::Ray::ConvertVec(line.start),
                Utility::Ray::ConvertVec(line.end),
                { line.col.r, line.col.g, line.col.b, line.col.a });
        
        EndMode3D();
    }

    EndTextureMode();
    
    return static_cast<int>(scene.debugShapes.Count() + scene.debugLines.Count());
}

void Rendering::Renderer::Blip(const RenderTexture2D& InTarget, const RenderTarget& InBuffer)
{
    PROFILE_GL();
    
    rlState::current.Reset();
    
    BeginTextureMode(InTarget);

    // Flip and blip
    const Rectangle sourceRec = {
        0.0f, 0.0f,
        static_cast<float>(InTarget.texture.width),
        -static_cast<float>(InTarget.texture.height)
    };

    CHECK_ASSERT(!InBuffer.GetTextures()[0].tex, "Tex nullptr");
    DrawTextureRec(
        *InBuffer.GetTextures()[0].tex,
        sourceRec,
        { 0, 0 }, 
        ::WHITE);

    EndTextureMode();
}

bool Rendering::Renderer::Bake(const BakedTexture& InTex)
{
    rlState::current.Reset();
    
    ShaderResource* shaderResource = InTex.Shader.Get().Get();
    CHECK_RETURN_LOG(!shaderResource, "Failed to find shader resource", false);
    const Shader* shader = shaderResource->Get();
    CHECK_RETURN_LOG(!shader, "Failed to get shader", false);

    FrameCommand frameCmd;
    frameCmd.fboID = InTex.target.GetFBO();
    frameCmd.size = InTex.target.Size();
    rlState::current.Set(frameCmd);

    ShaderCommand shaderCmd;
    shaderCmd.locs = shader->locs;
    shaderCmd.id = shader->id;
    shaderCmd.blendMode = -1;
    rlState::current.Set(shaderCmd);

    Vec4F rect = Vec4F(0, 0, 1, 1);
    SetValue(*shaderResource, ShaderResource::DefaultLoc::RECT, &rect, SHADER_UNIFORM_VEC4);
    
    DrawQuad();

    rlState::current.Reset();
    
    return true;
}
