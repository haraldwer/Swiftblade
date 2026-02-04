#pragma once
#include "../Uniforms/UniformBuffer.h"
#include "Resources/Material.h"
#include "Resources/Model.h"

namespace Rendering
{
    class Viewport;
    struct Scene;
    class CommandList;

    class SceneRenderer : public Utility::Singelton<SceneRenderer, true>
    {
    public:
        struct Entry
        {
            Scene* scene = nullptr;
            Viewport* viewport = nullptr;
        };
        
        void Enqueue(const Entry& InEntry)
        {
            CHECK_ASSERT(!InEntry.scene, "Invalid scene ptr");
            entries.push_back(InEntry);
        }
        
        void Render(CommandList& InOutList, Viewport& InMainViewport);
        
    private:
        Vector<Entry> entries;
        BufferCollection buffers;
        
        ResRM rm = ResRM("RM_Test.json");
        ResModel m = ResModel("Sponza/sponza.obj");
    };
}
