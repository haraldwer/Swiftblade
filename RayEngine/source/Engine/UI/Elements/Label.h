﻿#pragma once
#include "Element.h"
#include "UI/Resources/Font.h"

namespace UI
{
    class Label : public Element
    {
    public:
        explicit Label(const String& InText, const Vec2F& InCentering = Vec2F::Zero(), const UI::Transform& InTransform = Transform::Fill(), const ResFont& InFont = ResFont("Defaults/F_SourceCodePro.ttf"), const float InSize = 30.0f, const float InSpacing = 0.0f) :
            Element(InTransform), Centering(InCentering), Font(InFont), Text(InText), Size(InSize), Spacing(InSpacing) {}
        void Init() override;
        void Draw() override;
        
        void SetText(const String& InText);

    private:
        Vec2F Centering;
        ResFont Font;
        String Text;

        float Size = 20.0f;
        float Spacing = 0.0f;
        
        Vec2F CachedSize;
    };
}
