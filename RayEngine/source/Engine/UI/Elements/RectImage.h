#pragma once
#include "Image.h"

namespace UI
{
    class RectImage : public Image
    {
        TYPE_INFO(RectImage, Image)
        
    public:
        RectImage(const ResTexture& InTexture = String("Defaults/T_Missing.png"), const Margin& InMargin = 0.0f, const Transform& InTransform = Transform::Fill()) :
            Image(InTransform, InTexture), edges(InMargin) {}
        void Draw(Container& InOwner) override;

    private:
        Margin edges; 
    };
}
