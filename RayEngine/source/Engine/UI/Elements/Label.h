#pragma once
#include "Element.h"
#include "UI/Resources/Font.h"

namespace UI
{
    enum class LabelSize
    {
        TEXT = 20,
        HEADER = 35,
        TITLE = 50
    };

    struct LabelProperties
    {
        String text;
        float size = static_cast<float>(LabelSize::TEXT);
        ResFont font = ResFont("UI/F_LinLibertine_RB.ttf");

        float spacing = 0.0f;
        Vec4F color = Vec4F::One();
    };
    
    class Label : public Element
    {
        TYPE_INFO(Label, Element)

    public:
        Label(const Transform& InTransform = {}, const LabelProperties& InProperties = {}) : Element(InTransform), properties(InProperties) {}
        void Init(Container& InOwner) override;
        void Draw(Container& InOwner) override;
        Vec2F GetDesiredSize() const override;
        
        void SetText(const String& InText);
        String GetText() const { return properties.text; }

    private:
        void CacheSize(); 
        float GetScreenSize() const;
        
        LabelProperties properties;
        Vec2F cachedSize = {};
    };
}
