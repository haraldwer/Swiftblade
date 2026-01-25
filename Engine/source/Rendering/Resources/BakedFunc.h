#pragma once

namespace Rendering
{
    class ShaderResource;

    // TODO:
    // Sort out BakedFunc, BakedTexture and NoiseTexture
    
    class BakedFunc
    {
    public:
        bool Unload();
        void Bind(const String& InName, ShaderResource& InShader, int& InOutSlot, int InFilter) const;
        void Bake(const Vec2I& InResolution, const std::function<Vec4F(const Vec2F&, const Vec2I&)>& InEvalFunc);
        bool IsBaked() const { return baked; }

    private:
        // Make resource?
        bool baked = false;
    };
}