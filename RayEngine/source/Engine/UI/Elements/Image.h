﻿#pragma once
#include "Element.h"
#include "Rendering/Resources/Texture.h"

namespace UI
{
    class Image : public Element
    {
        TYPE_INFO(Image, Element)
        
    public:
        Image(const ResTexture& InTexture = String("Defaults/T_Missing.png"), const UI::Transform& InTransform = Transform::Fill()) :
            Element(InTransform), texture(InTexture) {}
        ~Image() override = default; 
        void Draw(Container& InOwner) override;
        
    protected:
        ResTexture texture;
    };
}
