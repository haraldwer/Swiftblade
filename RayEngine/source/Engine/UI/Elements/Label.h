#pragma once
#include "Element.h"
#include "Engine/UI/Resources/Font.h"

namespace UI
{
    class Label : public Element
    {
    public:
        Label(const String& InText, const Vec2F& InCentering = Vec2F::Zero(), const UI::Transform& InTransform = Transform::Fill(), const ResFont& InFont = ResFont("Defaults/F_SourceCodePro.ttf")) :
            Element(InTransform), Centering(InCentering), Text(InText), Font(InFont) {}
        void Init() override;
        Rect Draw(const Rect& InContainer) override;
    private:
        Vec2F Centering;
        ResFont Font;
        String Text;

        float Size = 20.0f;
        float Spacing = 0.0f;
        
        Vec2F CachedSize;
    };
}
