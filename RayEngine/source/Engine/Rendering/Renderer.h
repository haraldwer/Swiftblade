#pragma once

#include "Utility/Singelton.h"
#include "Scene.h"
#include "Config.h"

namespace Rendering
{
    class Renderer : public Utility::Singelton<Renderer, true>
    {

    public:

        void Init(); 
        void Deinit();

        bool ShouldClose();
        
        void BeginVirtualFrame(const RenderTexture2D* InTarget = nullptr);
        void RenderScenes(double InDelta);
        void EndVirtualFrame();

        void BeginFrame();
        void EndFrame();
        
        void Clear(); 
        void Push(const Scene& InScene);

        void ApplyConfig(const Config& InConfig);

        Vec2F GetWindowSize() const
        {
            return {
                static_cast<float>(CurrConfig.Width),
                static_cast<float>(CurrConfig.Height)
            };
        }
        
        Vec2F GetResolution() const
        {
            return {
                static_cast<float>(VirtualTarget.texture.width),
                static_cast<float>(VirtualTarget.texture.height)
            };
        }

    private:

        Config CurrConfig; 
        Vector<Scene> Scenes;
        RenderTexture2D VirtualTarget = RenderTexture2D();
    };
}

