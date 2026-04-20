#pragma once
#include "ButtonDefault.h"

namespace UI
{
    struct ToggleProperties
    {
        Vec4F toggleHoverColor = { 0, 0, 0, 0.1 };
        Vec4F toggleColor = { 0, 0, 0, 0.3 };
        bool defaultState = false;
    };
    
    class Toggle : public ButtonDefault
    {
        CLASS_INFO(Toggle, ButtonDefault)
        
    public:
        Toggle(const Transform& InTrans, const ButtonProperties& InButton = {}, const ToggleProperties& InProperties = {}) :
            ButtonDefault(InTrans, InButton), toggleProperties(InProperties), toggled(InProperties.defaultState) {}

        void Update(Container &InOwner) override;
        void DefaultAnim() override;
        void HoverAnim() override;

        bool IsToggled() const { return toggled; }
        void SetToggled(const bool InToggled) { toggled = InToggled; }
        String GetOption() const { return properties.option; }
        
    private:
        ToggleProperties toggleProperties;
        bool toggled = false;
    };
}
