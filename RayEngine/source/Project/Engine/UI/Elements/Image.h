#pragma once
#include "Element.h"
#include "Engine/Rendering/Resources/Texture.h"

namespace UI
{
    class Image : public Element
    {
    public:
        Image(const ResTexture& InTexture = String("Defaults/T_Missing.png"), const UI::Transform& InTransform = Transform::Fill()) :
            Element(InTransform), Texture(InTexture) {}
        ~Image() override = default; 
        void Draw() override;
        
    protected:
        ResTexture Texture;
    };
}
