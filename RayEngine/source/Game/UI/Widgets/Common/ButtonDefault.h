#pragma once
#include "UI/Elements/Container.h"

namespace UI
{
    struct ButtonProperties
    {
        String option;
        String text;
        
        Vec4F hoverColor = { 1, 1, 1, 0.7 };
        float hoverSpeed = 0.9f;
        Vec4F pressColor = { 1, 1, 1, 0.9 };
        float pressSpeed = 0.1f;
        Vec4F defaultColor = { 1, 1, 1, 0.5 };
        float defaultSpeed = 0.1f;
    };
    
    class ButtonDefault : public Container
    {
        CLASS_INFO(ButtonDefault, Container);
        
    public:
        ButtonDefault(const Transform& InTrans, const ButtonProperties& InProperties = {}) : Container(InTrans), properties(InProperties) {}
        void Init(Container &InOwner) override;
        void Update(Container &InOwner) override;
        bool IsHovered() const override { return Element::IsHovered(); }
    
    protected:

        virtual void HoverAnim();
        virtual void PressAnim();
        virtual void DefaultAnim();

        void LerpTowards(Vec2F InPos, Vec4F InCol, float InSpeed);
        
        ButtonProperties properties;
        ElementID root = -1;
    };
}
