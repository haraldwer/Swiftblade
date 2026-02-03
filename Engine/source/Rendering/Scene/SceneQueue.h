#pragma once
#include "Scene.h"

namespace Rendering
{
    class Viewport;

    class SceneQueue : public Utility::Singelton<SceneQueue, true>
    {
        friend class Manager;
    public:
        
        struct Entry
        {
            Scene* scene = nullptr;
            Viewport* target = nullptr;
        };
        
        // Queue scene for rendering
        // Is cleared every frame
        void Enqueue(const Entry& InEntry)
        {
            CHECK_ASSERT(!InEntry.scene, "Invalid scene ptr");
            entries.push_back(InEntry);
        }
        
    private:
        Vector<Entry> entries;
    };
}
