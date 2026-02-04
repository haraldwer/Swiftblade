#include "SceneRenderer.h"

#include "Scene.h"
#include "Commands/Command.h"
#include "Commands/CommandList.h"
#include "Viewport/Viewport.h"

void Rendering::SceneRenderer::Render(CommandList& InOutList, Viewport& InMainViewport)
{
    for (Entry& entry : entries)
    {
        CHECK_ASSERT(!entry.scene, "Invalid scene");
        
        entry.scene->Build();
        
        Viewport& viewport = entry.viewport ? 
            *entry.viewport : InMainViewport;
        FrameTargets& targets = viewport.GetTargets();

        CameraInstance &cam = entry.scene->camera;
        Vec2F res = viewport.Size().To<float>();
        Mat4F view = cam.GetViewMatrix();
        Mat4F proj = cam.GetProjectionMatrix(res);
        Mat4F viewproj = Mat4F::GetInverse(view) * proj;

        BufferGroup& buffer = buffers.GetGroup(0);
        buffer.Set(0, viewproj);
        
        Command command("MainCommand");
        command.material = rm;
        command.targets = { &targets.frame };
        command.clear = true;
        command.clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
        command.depthTarget = &targets.depth;
        command.writeDepth = true;
        command.transforms = {}; // Assumes one mesh
        command.model = m; // Assumes fullscreen pass
        command.uniforms = {}; // Contains textures and buffers
        command.buffers = &buffers;
        InOutList.Add(command);
    }
    entries.clear();
}
