#pragma once
#include "Element.h"
#include "Rendering/Resources/Texture.h"

namespace UI
{
    struct ImageProperties
    {
        ResTexture texture = ResTexture("Defaults/T_Missing.png");
        Vec4F color = Vec4F::One();
    };
    
    class Image : public Element
    {
        TYPE_INFO(Image, Element)

    public:
        
        Image(const Transform& InTransform = Transform::Fill(), const ImageProperties& InProperties = ImageProperties()) : Element(InTransform), properties(InProperties) {}
        void Draw(Container& InOwner) override;

        Vec2F GetDesiredSize() const override;
        
    protected:
        ImageProperties properties;
    };
}
