#pragma once
#include "Container.h"
#include "Label.h"

namespace UI
{
    struct TextboxProperties
    {
        LabelProperties label;
        String defaultText;
        String emptyText;
        bool multiline = false;
        bool wrap = false;
        int maxChars = 0;
        Vec2F centering = {}; 
    };
    
    class Textbox : public Container
    {
        CLASS_INFO(Textbox, Container)

    public:
        Textbox(const Transform& InTransform = {}, const TextboxProperties& InProperties = {}, const Background& InBg = {});
        void Update(Container &InOwner) override;
        bool IsHovered() const override;
        bool IsCommitted() const { return committed; }
        
        String GetText() const { return text; }
        void SetText(const String& InText) { text = InText; }
        
    private:
        TextboxProperties properties;
        ElementID label = -1;
        String text;
        
        bool focused = false;
        float cursorTime = 0;
        bool cursorShown = false;
        bool committed = false; 
    };
}
