#pragma once
#include "Element.h"
#include "Rendering/Resources/Texture.h"

namespace UI
{
    class Image : public Element
    {
        TYPE_INFO(Image, Element)
        
    public:
        Image(const ResTexture& InTexture = String("Defaults/T_Missing.png"), const UI::Transform& InTransform = Transform::Fill(), const Vec4F InColor = Vec4F::One()) :
            Element(InTransform), texture(InTexture), imageColor(InColor) {}
        ~Image() override = default; 
        void Draw(Container& InOwner) override;
        
    protected:
        ResTexture texture;
        Vec4F imageColor = Vec4F::One();
    };
}
