#pragma once
#include "Element.h"
#include "UI/Resources/Font.h"

namespace UI
{
    class Label : public Element
    {
        TYPE_INFO(Label, Element)
        
    public:
        explicit Label(const String& InText, const Vec2F& InCentering = Vec2F::Zero(), const Transform& InTransform = Transform::Fill(), const ResFont& InFont = ResFont("Defaults/F_FiraCode.ttf"), const float InSize = 30.0f, const float InSpacing = 0.0f) :
            Element(InTransform), centering(InCentering), font(InFont), text(InText), size(InSize), spacing(InSpacing) {}
        void Init(Container& InOwner) override;
        void Draw(Container& InOwner) override;
        
        void SetText(const String& InText);

    private:
        Vec2F centering = {};
        ResFont font = {};
        String text = {};

        float size = 20.0f;
        float spacing = 0.0f;
        
        Vec2F cachedSize = {};
    };
}
