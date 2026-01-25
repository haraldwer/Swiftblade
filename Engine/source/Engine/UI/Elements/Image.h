#pragma once
#include "Element.h"
#include "Resources/Texture.h"

namespace UI
{
    struct ImageProperties
    {
        ResTexture texture = ResTexture("Defaults/T_Missing.png");
        Vec4F color = Vec4F::One();
    };
    
    class Image : public Element
    {
        CLASS_INFO(Image, Element)

    public:
        
        Image(const Transform& InTransform = Transform::Fill(), const ImageProperties& InProperties = ImageProperties()) : Element(InTransform), properties(InProperties) {}
        void Draw(Container& InOwner) override;

        void SetProperties(const ImageProperties& InProperties);
        Vec2F GetDesiredSize() const override;
        
    protected:
        ImageProperties properties;
    };
}
