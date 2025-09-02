#pragma once
#include "Element.h"
#include "UI/Resources/Font.h"

namespace UI
{
    enum class LabelSize
    {
        TEXT = 30,
        HEADING = 50,
        TITLE = 150
    };
    
    class Label : public Element
    {
        TYPE_INFO(Label, Element)
        
    public:
        explicit Label(const Transform& InTransform, const String& InText, const float InSize = 30.0f, const ResFont& InFont = ResFont("UI/F_LinLibertine_RB.ttf"), const float InSpacing = 0.0f, const Vec2F& InCentering = Vec2F::Zero()) :
            Element(InTransform), centering(InCentering), font(InFont), text(InText), size(InSize), spacing(InSpacing) {}
        void Init(Container& InOwner) override;
        void Draw(Container& InOwner) override;
        Vec2F GetDesiredSize() const override;
        
        void SetText(const String& InText);
        String GetText() const { return text; }

    private:
        void CacheSize(); 
        float GetScreenSize() const;

        Vec2F centering = {};
        ResFont font = {};
        String text = {};

        float size = 20.0f;
        float spacing = 0.0f;
        Vec4F color = Vec4F::One();
        
        Vec2F cachedSize = {};
    };
}
