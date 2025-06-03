#pragma once
#include "Image.h"

namespace UI
{
    class RectImage : public Image
    {
    public:
        RectImage(const ResTexture& InTexture = String("Defaults/T_Missing.png"), const Margin& InMargin = 0.0f, const UI::Transform& InTransform = Transform::Fill()) :
            Image(InTexture, InTransform), edges(InMargin) {}
        void Draw() override;

    private:
        Margin edges; 
    };
}
